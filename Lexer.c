#include "Lexer.h"

#include <ctype.h>

#include "SourceFile.h"

static char Lexer_PeekChar(const Lexer* self);
static char Lexer_ConsumeChar(Lexer* self);

static bool IsDigit(const char c)
{
	return c >= '0' && c <= '9';
}

static bool IsBinaryDigit(const char c)
{
	return c == '0' || c == '1';
}

static bool IsOctalDigit(const char c)
{
	return c >= '0' && c <= '7';
}

static bool IsHexDigit(const char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

Lexer Lexer_Create(const SourceFile* source, CompilerErrorList* errorList)
{
	return (Lexer) {
		.source = source,
		.position = 0,
		.line = 1,
		.column = 1,
		.errors = errorList,
	};
}

Token Lexer_GetNextToken(Lexer* self, const bool includeWhitespace, const bool includeComments)
{
	size_t startPosition, startLine, startColumn;
restart:
	startPosition = self->position;
	startLine = self->line;
	startColumn = self->column;
	if (self->position >= String_Length(self->source->content))
	{
		return Token_Create(TOKEN_EOF, SourceLocation_Create(self->source, startPosition, 0, startLine, startColumn), (Token_Data) { 0 });
	}

	// Whitespace
	char c = Lexer_ConsumeChar(self);
	if (c == ' ' || c == '\t' || c == '\n')
	{
		while (((c = Lexer_PeekChar(self))) && (c == ' ' || c == '\t' || c == '\n'))
			Lexer_ConsumeChar(self);

		if (includeWhitespace)
		{
			return Token_Create(TOKEN_WHITESPACE,
			                    SourceLocation_Create(self->source, startPosition, self->position - startPosition, startLine, startColumn),
			                    (Token_Data) { 0 });
		}

		goto restart;
	}

	char buf3[3];
	buf3[0] = c;
	buf3[1] = self->position < self->source->content->length ? String_AsCString(self->source->content)[self->position] : '\0';
	buf3[2] = self->position + 1 < self->source->content->length ? String_AsCString(self->source->content)[self->position + 1] : '\0';

	// Comments
	if (buf3[0] == '/' && buf3[1] == '/')
	{
		// Single-line comment
		Lexer_ConsumeChar(self); // Consume second '/'

		while (((c = Lexer_PeekChar(self))) && c != '\n')
			Lexer_ConsumeChar(self);

		if (includeComments)
		{
			return Token_Create(TOKEN_COMMENT_SINGLELINE,
			                    SourceLocation_Create(self->source, startPosition, self->position - startPosition, startLine, startColumn),
			                    (Token_Data) { 0 });
		}

		goto restart;
	}

	if (buf3[0] == '/' && buf3[1] == '*')
	{
		// Multi-line comment
		Lexer_ConsumeChar(self); // Consume '*'

		while (true)
		{
			c = Lexer_ConsumeChar(self);
			if (c == '\0')
			{
				const SourceLocation errorLoc = SourceLocation_Create(self->source, startPosition, self->position - startPosition, startLine, startColumn);
				CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_UnterminatedComment, errorLoc));
				break;
			}

			if (c == '*' && Lexer_PeekChar(self) == '/')
			{
				Lexer_ConsumeChar(self); // consume '/'
				break;
			}
		}

		if (includeComments)
		{
			return Token_Create(TOKEN_COMMENT_MULTILINE,
			                    SourceLocation_Create(self->source, startPosition, self->position - startPosition, startLine, startColumn),
			                    (Token_Data) { 0 });
		}

		goto restart;
	}

	// Numeric literals (integer and floating-point)
	// Starts with a digit or a '.' followed by a digit
	if (IsDigit(c) || (c == '.' && IsDigit(buf3[1])))
	{
		// Does the number have an integer part (does it start with a digit)?
		const bool hasIntegerPart = c != '.';

		// Is it hexadecimal, binary, or decimal/octal?
		const bool isHex = c == '0' && (buf3[1] == 'x' || buf3[1] == 'X');
		const bool isBin = c == '0' && (buf3[1] == 'b' || buf3[1] == 'B');
		const bool isDecimalOrOctal = !isHex && !isBin;

		// Keep track of whether the number consists of valid octal digits
		bool isValidOctal = true;

		// Determine the start of the integer part, consume radix prefix
		size_t intStart = self->position - 1;
		if (isHex || isBin)
		{
			intStart += 2;
			Lexer_ConsumeChar(self);
		}

		// Consume integer part, if any
		if (hasIntegerPart)
		{
			while (((c = Lexer_PeekChar(self))) && ((isDecimalOrOctal && IsDigit(c)) || (isBin && IsBinaryDigit(c)) || (isHex && IsHexDigit(c))))
			{
				if (c > '7' && isDecimalOrOctal)
					isValidOctal = false;
				Lexer_ConsumeChar(self);
			}
		}
		const ConstCharSpan integerPart = ConstCharSpan_SubSpan(String_AsConstCharSpan(self->source->content), intStart, self->position - intStart);

		// Check for fractional part or exponent, to distinguish integer vs floating-point literal
		char nextCharLower = (char)tolower(Lexer_PeekChar(self));
		const bool isInteger = hasIntegerPart && nextCharLower != '.' && nextCharLower != FLOATING_POINT_EXONENT_CHAR(isHex);

		// Invalid (floating-point literal cannot be binary)
		if (!isInteger && isBin)
		{
			const SourceLocation errorLoc = SourceLocation_Create(self->source, startPosition, self->position - startPosition, startLine, startColumn);
			CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_InvalidNumericLiteral, errorLoc));
		}

		// Integer literal
		if (isInteger)
		{
			// Read suffix
			buf3[0] = (char)tolower(Lexer_PeekChar(self));
			buf3[1] = self->position + 1 < self->source->content->length ? (char)tolower(String_AsCString(self->source->content)[self->position + 1]) : '\0';
			buf3[2] = self->position + 2 < self->source->content->length ? (char)tolower(String_AsCString(self->source->content)[self->position + 2]) : '\0';

			// Determine type based on suffix
			Token_LiteralInteger_Type type = TOKEN_LITERAL_INTEGER_TYPE_INT;
			if ((buf3[0] == 'u' && buf3[1] == 'l' && buf3[2] == 'l') || (buf3[0] == 'l' && buf3[1] == 'l' && buf3[2] == 'u')) // ULL / LLU
			{
				type = TOKEN_LITERAL_INTEGER_TYPE_UNSIGNEDLONGLONG;
				Lexer_ConsumeChar(self);
				Lexer_ConsumeChar(self);
				Lexer_ConsumeChar(self);
			}
			else if (buf3[0] == 'l' && buf3[1] == 'l') // LL / ll
			{
				type = TOKEN_LITERAL_INTEGER_TYPE_LONGLONG;
				Lexer_ConsumeChar(self);
				Lexer_ConsumeChar(self);
			}
			else if (buf3[0] == 'u' && buf3[1] == 'l') // UL
			{
				type = TOKEN_LITERAL_INTEGER_TYPE_UNSIGNEDLONG;
				Lexer_ConsumeChar(self);
				Lexer_ConsumeChar(self);
			}
			else if (buf3[0] == 'l') // L / l
			{
				type = TOKEN_LITERAL_INTEGER_TYPE_LONG;
				Lexer_ConsumeChar(self);
			}
			else if (buf3[0] == 'u') // U / u
			{
				type = TOKEN_LITERAL_INTEGER_TYPE_UNSIGNEDINT;
				Lexer_ConsumeChar(self);
			}

			// Determine base
			size_t base = 10;
			if (isHex)
				base = 16;
			else if (isBin)
				base = 2;
			else if (integerPart.length > 1 && integerPart.data[0] == '0')
				base = 8;

			// Invalid octal literal
			if (base == 8 && !isValidOctal)
			{
				const SourceLocation errorLoc = SourceLocation_Create(self->source, startPosition, self->position - startPosition, startLine, startColumn);
				CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_InvalidNumericLiteral, errorLoc));
			}

			return Token_Create(TOKEN_LITERAL_INTEGER,
			                    SourceLocation_Create(self->source, startPosition, self->position - startPosition, startLine, startColumn),
			                    (Token_Data) { .literalInteger = { integerPart, base, type } });
		}

		// Floating-point literal

		// Consume fractional part (if any)
		bool hasFractionalPart = false;
		ConstCharSpan fractionalPart = ConstCharSpan_Empty;
		if (!hasIntegerPart || nextCharLower == '.')
		{
			// Consume '.' (only consume if there is an integer part)
			if (hasIntegerPart)
				Lexer_ConsumeChar(self);

			// Consume fractional digits
			const size_t fracStart = self->position;
			while (((c = Lexer_PeekChar(self))) && IsHexDigit(c))
				Lexer_ConsumeChar(self);

			// Determine if there actually is a fractional part
			hasFractionalPart = self->position > fracStart;
			fractionalPart = ConstCharSpan_SubSpan(String_AsConstCharSpan(self->source->content), fracStart, self->position - fracStart);
		}

		// Consume exponent part, if any
		bool hasExponent = false;
		bool exponentIsNegative = false;
		ConstCharSpan exponent = ConstCharSpan_Empty;
		nextCharLower = (char)tolower(Lexer_PeekChar(self));
		if (nextCharLower == FLOATING_POINT_EXONENT_CHAR(isHex))
		{
			Lexer_ConsumeChar(self); // Consume exponent character

			if (Lexer_PeekChar(self) == '+' || Lexer_PeekChar(self) == '-')
				exponentIsNegative = Lexer_ConsumeChar(self) == '-'; // Consume '+' or '-'

			const size_t expStart = self->position;
			while (((c = Lexer_PeekChar(self))) && IsDigit(c))
				Lexer_ConsumeChar(self);

			hasExponent = self->position > expStart;
			if (!hasExponent)
			{
				const SourceLocation errorLoc = SourceLocation_Create(self->source, startPosition, self->position - startPosition, startLine, startColumn);
				CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_InvalidNumericLiteral, errorLoc));
			}

			exponent = ConstCharSpan_SubSpan(String_AsConstCharSpan(self->source->content), expStart, self->position - expStart);
		}

		Token_LiteralFloat_Type suffix = TOKEN_LITERAL_FLOAT_TYPE_DOUBLE;
		// Consume suffix
		char suffixChar = (char)tolower(Lexer_PeekChar(self));
		if (suffixChar == 'd')
		{
			Lexer_ConsumeChar(self);
			suffixChar = (char)tolower(Lexer_PeekChar(self));

			switch (suffixChar)
			{
				case 'f':
					suffix = TOKEN_LITERAL_FLOAT_TYPE_DECIMAL32;
					break;
				case 'd':
					suffix = TOKEN_LITERAL_FLOAT_TYPE_DECIMAL64;
					break;
				case 'l':
					suffix = TOKEN_LITERAL_FLOAT_TYPE_DECIMAL128;
					break;
				default:
				{
					const SourceLocation errorLoc = SourceLocation_Create(self->source, startPosition, self->position - startPosition, startLine, startColumn);
					CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_InvalidNumericLiteral, errorLoc));
				}
			}

			Lexer_ConsumeChar(self);
		}
		else if (suffixChar == 'f' || suffixChar == 'l')
		{
			Lexer_ConsumeChar(self);
			switch (suffixChar)
			{
				case 'f':
				{
					buf3[0] = (char)tolower(Lexer_PeekChar(self));
					buf3[1] = self->position + 1 < self->source->content->length
						          ? (char)tolower(String_AsCString(self->source->content)[self->position + 1])
						          : '\0';
					const bool isF16 = buf3[0] == '1' && buf3[1] == '6';
					const bool isF32 = buf3[0] == '3' && buf3[1] == '2';
					const bool isF64 = buf3[0] == '6' && buf3[1] == '4';
					if (isF16)
						suffix = TOKEN_LITERAL_FLOAT_TYPE_BINARY16;
					else if (isF32)
						suffix = TOKEN_LITERAL_FLOAT_TYPE_BINARY32;
					else if (isF64)
						suffix = TOKEN_LITERAL_FLOAT_TYPE_BINARY64;
					else
						suffix = TOKEN_LITERAL_FLOAT_TYPE_FLOAT;

					if (suffix != TOKEN_LITERAL_FLOAT_TYPE_FLOAT)
					{
						Lexer_ConsumeChar(self);
						Lexer_ConsumeChar(self);
					}
					break;
				}
				case 'l':
					suffix = TOKEN_LITERAL_FLOAT_TYPE_LONGDOUBLE;
					break;
				default:
					;
			}
		}

		return Token_Create(TOKEN_LITERAL_FLOAT,
		                    SourceLocation_Create(self->source, startPosition, self->position - startPosition, startLine, startColumn),
		                    (Token_Data) {
			                    .literalDecimalFloat =
			                    {
				                    isHex,
				                    hasIntegerPart,
				                    integerPart,
				                    hasFractionalPart,
				                    fractionalPart,
				                    hasExponent,
				                    exponentIsNegative,
				                    exponent,
				                    suffix
			                    }
		                    });
	}

	// String and character literals
	if (c == '"' || c == '\'')
	{
		const char quoteChar = c;
		while (true)
		{
			c = Lexer_ConsumeChar(self);

			if (c == '\0' || c == '\n')
			{
				const SourceLocation errorLoc = SourceLocation_Create(self->source, startPosition, self->position - startPosition, startLine, startColumn);
				CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_UnterminatedStringLiteral, errorLoc));
				goto restart;
			}

			// Closing quote
			if (c == quoteChar)
				break;

			// Escape sequence
			if (c == '\\')
				Lexer_ConsumeChar(self);
		}

		const Token_Type type = quoteChar == '"' ? TOKEN_LITERAL_STRING : TOKEN_LITERAL_CHAR;
		return Token_Create(type,
		                    SourceLocation_Create(self->source, startPosition, self->position - startPosition, startLine, startColumn),
		                    (Token_Data) { 0 });
	}

	// Punctuators
	for (size_t i = 0; i < sizeof(punctuators) / sizeof(TokenStringMapEntry); i++)
	{
		const TokenStringMapEntry* entry = &punctuators[i];

		const size_t len = strlen(entry->str);
		if (strncmp(buf3, entry->str, len) == 0)
		{
			for (size_t k = 1; k < len; k++)
				Lexer_ConsumeChar(self);

			return Token_Create(entry->type,
			                    SourceLocation_Create(self->source, startPosition, len, startLine, startColumn),
			                    (Token_Data) { 0 });
		}
	}

	// Identifiers and keywords
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
	{
		while (((c = Lexer_PeekChar(self))) && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_'))
			Lexer_ConsumeChar(self);

		const SourceLocation lexeme = SourceLocation_Create(self->source, startPosition, self->position - startPosition, startLine, startColumn);

		for (size_t i = 0; i < sizeof(keywords) / sizeof(TokenStringMapEntry); i++)
		{
			const TokenStringMapEntry* entry = &keywords[i];

			if (strlen(entry->str) == lexeme.snippet.length && memcmp(lexeme.snippet.data, entry->str, lexeme.snippet.length) == 0)
				return Token_Create(entry->type,
				                    lexeme,
				                    (Token_Data) { 0 });
		}

		return Token_Create(TOKEN_IDENTIFIER,
		                    lexeme,
		                    (Token_Data) { 0 });
	}

	// Unexpected character
	return Token_Create(TOKEN_UNEXPECTED,
	                    SourceLocation_Create(self->source, startPosition, 1, startLine, startColumn),
	                    (Token_Data) { 0 });
}

char Lexer_PeekChar(const Lexer* self)
{
	if (self->position >= self->source->content->length)
		return '\0';

	const char c = String_AsCString(self->source->content)[self->position];

	if (c == '\r')
		return '\n';

	return c;
}

char Lexer_ConsumeChar(Lexer* self)
{
	if (self->position >= self->source->content->length)
		return '\0';

	const char c = String_AsCString(self->source->content)[self->position++];
	if (c == '\r')
	{
		if (self->position < self->source->content->length && String_AsCString(self->source->content)[self->position] == '\n')
			self->position++;

		self->line++;
		self->column = 1;
		return '\n';
	}

	if (c == '\n')
	{
		self->line++;
		self->column = 1;
	}
	else
	{
		self->column++;
	}

	return c;
}
