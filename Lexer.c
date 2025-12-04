#include "Lexer.h"

#include <ctype.h>

#include "Util/Macros.h"
#include "Util/Managed.h"

typedef struct
{
	const char* str;
	const Token_Type type;
} TokenStringMapEntry;

#define FLOATING_POINT_EXONENT_CHAR(isHex) ((isHex) ? 'p' : 'e')

static const TokenStringMapEntry punctuators[] = {
	// Three character punctuators
	{ "<<=", TOKEN_PUNCTUATOR_LESS_LESS_EQUAL },
	{ ">>=", TOKEN_PUNCTUATOR_GREATER_GREATER_EQUAL },
	//3,  Two character punctuators
	{ "==", TOKEN_PUNCTUATOR_EQUAL_EQUAL },
	{ "!=", TOKEN_PUNCTUATOR_EXCLAMATION_EQUAL },
	{ "<=", TOKEN_PUNCTUATOR_LESS_EQUAL },
	{ ">=", TOKEN_PUNCTUATOR_GREATER_EQUAL },
	{ "++", TOKEN_PUNCTUATOR_PLUS_PLUS },
	{ "--", TOKEN_PUNCTUATOR_MINUS_MINUS },
	{ "+=", TOKEN_PUNCTUATOR_PLUS_EQUAL },
	{ "-=", TOKEN_PUNCTUATOR_MINUS_EQUAL },
	{ "*=", TOKEN_PUNCTUATOR_ASTERISK_EQUAL },
	{ "/=", TOKEN_PUNCTUATOR_SLASH_EQUAL },
	{ "%=", TOKEN_PUNCTUATOR_PERCENT_EQUAL },
	{ "&=", TOKEN_PUNCTUATOR_AMPERSAND_EQUAL },
	{ "|=", TOKEN_PUNCTUATOR_PIPE_EQUAL },
	{ "^=", TOKEN_PUNCTUATOR_CARET_EQUAL },
	{ "&&", TOKEN_PUNCTUATOR_AMPERSAND_AMPERSAND },
	{ "||", TOKEN_PUNCTUATOR_PIPE_PIPE },
	{ "<<", TOKEN_PUNCTUATOR_LESS_LESS },
	{ ">>", TOKEN_PUNCTUATOR_GREATER_GREATER },
	// Single character punctuators
	{ "=", TOKEN_PUNCTUATOR_EQUAL },
	{ "+", TOKEN_PUNCTUATOR_PLUS },
	{ "-", TOKEN_PUNCTUATOR_MINUS },
	{ "*", TOKEN_PUNCTUATOR_ASTERISK },
	{ "/", TOKEN_PUNCTUATOR_SLASH },
	{ "%", TOKEN_PUNCTUATOR_PERCENT },
	{ "&", TOKEN_PUNCTUATOR_AMPERSAND },
	{ "|", TOKEN_PUNCTUATOR_PIPE },
	{ "^", TOKEN_PUNCTUATOR_CARET },
	{ "~", TOKEN_PUNCTUATOR_TILDE },
	{ "!", TOKEN_PUNCTUATOR_EXCLAMATION },
	{ "#", TOKEN_PUNCTUATOR_HASH },
	{ "<", TOKEN_PUNCTUATOR_LESS },
	{ ">", TOKEN_PUNCTUATOR_GREATER },
	{ ".", TOKEN_PUNCTUATOR_PERIOD },
	{ ",", TOKEN_PUNCTUATOR_COMMA },
	{ ";", TOKEN_PUNCTUATOR_SEMICOLON },
	{ "(", TOKEN_PUNCTUATOR_PARENOPEN },
	{ ")", TOKEN_PUNCTUATOR_PARENCLOSE },
	{ "{", TOKEN_PUNCTUATOR_BRACEOPEN },
	{ "}", TOKEN_PUNCTUATOR_BRACECLOSE },
	{ "[", TOKEN_PUNCTUATOR_BRACKETOPEN },
	{ "]", TOKEN_PUNCTUATOR_BRACKETCLOSE },
	{ "\"", TOKEN_PUNCTUATOR_DOUBLEQUOTE },
	{ "\'", TOKEN_PUNCTUATOR_SINGLEQUOTE },
};

static const TokenStringMapEntry keywords[] = {

	// Keywords: types
	{ "char", TOKEN_KEYWORD_CHAR },
	{ "double", TOKEN_KEYWORD_DOUBLE },
	{ "float", TOKEN_KEYWORD_FLOAT },
	{ "int", TOKEN_KEYWORD_INT },
	{ "long", TOKEN_KEYWORD_LONG },
	{ "short", TOKEN_KEYWORD_SHORT },
	{ "void", TOKEN_KEYWORD_VOID },

	// Keywords: qualifiers
	{ "const", TOKEN_KEYWORD_CONST },
	{ "volatile", TOKEN_KEYWORD_VOLATILE },
	{ "signed", TOKEN_KEYWORD_SIGNED },
	{ "unsigned", TOKEN_KEYWORD_UNSIGNED },
	{ "restrict", TOKEN_KEYWORD_RESTRICT },

	// Keywords: storage classes
	{ "auto", TOKEN_KEYWORD_AUTO },
	{ "extern", TOKEN_KEYWORD_EXTERN },
	{ "register", TOKEN_KEYWORD_REGISTER },
	{ "static", TOKEN_KEYWORD_STATIC },
	{ "typedef", TOKEN_KEYWORD_TYPEDEF },

	// Keywords: function specifiers
	{ "inline", TOKEN_KEYWORD_INLINE },

	// Keywords: structures
	{ "struct", TOKEN_KEYWORD_STRUCT },
	{ "union", TOKEN_KEYWORD_UNION },
	{ "enum", TOKEN_KEYWORD_ENUM },

	// Keywords: control flow
	{ "if", TOKEN_KEYWORD_IF },
	{ "else", TOKEN_KEYWORD_ELSE },
	{ "switch", TOKEN_KEYWORD_SWITCH },
	{ "case", TOKEN_KEYWORD_CASE },
	{ "default", TOKEN_KEYWORD_DEFAULT },

	// Keywords: loops/jumps
	{ "for", TOKEN_KEYWORD_FOR },
	{ "do", TOKEN_KEYWORD_DO },
	{ "while", TOKEN_KEYWORD_WHILE },
	{ "break", TOKEN_KEYWORD_BREAK },
	{ "continue", TOKEN_KEYWORD_CONTINUE },
	{ "goto", TOKEN_KEYWORD_GOTO },
	{ "return", TOKEN_KEYWORD_RETURN },

	// Keywords: operators
	{ "", TOKEN_KEYWORD_SIZEOF },
};

static const char* const tokenTypeName[TOKEN_MAX] = {

	"TOKEN_EOF",
	"TOKEN_UNEXPECTED",
	"TOKEN_WHITESPACE",
	"TOKEN_COMMENT_SINGLELINE",
	"TOKEN_COMMENT_MULTILINE",

	"TOKEN_LITERAL_INTEGER",
	"TOKEN_LITERAL_DECIMALFLOAT",
	"TOKEN_LITERAL_STRING",

	"TOKEN_IDENTIFIER",

	// Keywords: types
	"TOKEN_KEYWORD_CHAR",
	"TOKEN_KEYWORD_DOUBLE",
	"TOKEN_KEYWORD_FLOAT",
	"TOKEN_KEYWORD_INT",
	"TOKEN_KEYWORD_LONG",
	"TOKEN_KEYWORD_SHORT",
	"TOKEN_KEYWORD_VOID",

	// Keywords: qualifiers
	"TOKEN_KEYWORD_CONST",
	"TOKEN_KEYWORD_VOLATILE",
	"TOKEN_KEYWORD_SIGNED",
	"TOKEN_KEYWORD_UNSIGNED",
	"TOKEN_KEYWORD_RESTRICT",

	// Keywords: storage classes
	"TOKEN_KEYWORD_AUTO",
	"TOKEN_KEYWORD_EXTERN",
	"TOKEN_KEYWORD_REGISTER",
	"TOKEN_KEYWORD_STATIC",
	"TOKEN_KEYWORD_TYPEDEF",

	// Keywords: function specifiers
	"TOKEN_KEYWORD_INLINE",

	// Keywords: structures
	"TOKEN_KEYWORD_STRUCT",
	"TOKEN_KEYWORD_UNION",
	"TOKEN_KEYWORD_ENUM",

	// Keywords: control flow
	"TOKEN_KEYWORD_IF",
	"TOKEN_KEYWORD_ELSE",
	"TOKEN_KEYWORD_SWITCH",
	"TOKEN_KEYWORD_CASE",
	"TOKEN_KEYWORD_DEFAULT",

	// Keywords: loops/jumps
	"TOKEN_KEYWORD_FOR",
	"TOKEN_KEYWORD_DO",
	"TOKEN_KEYWORD_WHILE",
	"TOKEN_KEYWORD_BREAK",
	"TOKEN_KEYWORD_CONTINUE",
	"TOKEN_KEYWORD_GOTO",
	"TOKEN_KEYWORD_RETURN",

	// Keywords: operators
	"TOKEN_KEYWORD_SIZEOF",

	// Punctuators: single-character
	"TOKEN_PUNCTUATOR_EQUAL",
	"TOKEN_PUNCTUATOR_PLUS",
	"TOKEN_PUNCTUATOR_MINUS",
	"TOKEN_PUNCTUATOR_ASTERISK",
	"TOKEN_PUNCTUATOR_SLASH",
	"TOKEN_PUNCTUATOR_PERCENT",
	"TOKEN_PUNCTUATOR_AMPERSAND",
	"TOKEN_PUNCTUATOR_PIPE",
	"TOKEN_PUNCTUATOR_CARET",
	"TOKEN_PUNCTUATOR_TILDE",
	"TOKEN_PUNCTUATOR_EXCLAMATION",
	"TOKEN_PUNCTUATOR_HASH",
	"TOKEN_PUNCTUATOR_LESS",
	"TOKEN_PUNCTUATOR_GREATER",
	"TOKEN_PUNCTUATOR_PERIOD",
	"TOKEN_PUNCTUATOR_COMMA",
	"TOKEN_PUNCTUATOR_SEMICOLON",
	"TOKEN_PUNCTUATOR_PARENOPEN",
	"TOKEN_PUNCTUATOR_PARENCLOSE",
	"TOKEN_PUNCTUATOR_BRACEOPEN",
	"TOKEN_PUNCTUATOR_BRACECLOSE",
	"TOKEN_PUNCTUATOR_BRACKETOPEN",
	"TOKEN_PUNCTUATOR_BRACKETCLOSE",
	"TOKEN_PUNCTUATOR_DOUBLEQUOTE",
	"TOKEN_PUNCTUATOR_SINGLEQUOTE",

	// Punctuators: two-character
	"TOKEN_PUNCTUATOR_EQUAL_EQUAL",
	"TOKEN_PUNCTUATOR_EXCLAMATION_EQUAL",
	"TOKEN_PUNCTUATOR_LESS_EQUAL",
	"TOKEN_PUNCTUATOR_GREATER_EQUAL",
	"TOKEN_PUNCTUATOR_PLUS_PLUS",
	"TOKEN_PUNCTUATOR_MINUS_MINUS",
	"TOKEN_PUNCTUATOR_PLUS_EQUAL",
	"TOKEN_PUNCTUATOR_MINUS_EQUAL",
	"TOKEN_PUNCTUATOR_ASTERISK_EQUAL",
	"TOKEN_PUNCTUATOR_SLASH_EQUAL",
	"TOKEN_PUNCTUATOR_PERCENT_EQUAL",
	"TOKEN_PUNCTUATOR_AMPERSAND_EQUAL",
	"TOKEN_PUNCTUATOR_PIPE_EQUAL",
	"TOKEN_PUNCTUATOR_CARET_EQUAL",
	"TOKEN_PUNCTUATOR_AMPERSAND_AMPERSAND",
	"TOKEN_PUNCTUATOR_PIPE_PIPE",
	"TOKEN_PUNCTUATOR_LESS_LESS",
	"TOKEN_PUNCTUATOR_GREATER_GREATER",

	// Punctuators: three-character
	"TOKEN_PUNCTUATOR_LESS_LESS_EQUAL",
	"TOKEN_PUNCTUATOR_GREATER_GREATER_EQUAL",
};

static char Lexer_PeekChar(const Lexer* self);
static char Lexer_ReadChar(Lexer* self);

always_inline bool IsDigit(const char c)
{
	return c >= '0' && c <= '9';
}

always_inline bool IsBinaryDigit(const char c)
{
	return c == '0' || c == '1';
}

always_inline bool IsOctalDigit(const char c)
{
	return c >= '0' && c <= '7';
}

always_inline bool IsHexDigit(const char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

void Lexer_Init(Lexer* self, const ConstCharSpan source)
{
	self->source = source;
	self->position = 0;
	self->line = 1;
	self->column = 1;
}

Token Lexer_GetNextToken(Lexer* self, const bool includeWhitespace, const bool includeComments)
{
restart:
	const size_t startPosition = self->position;
	const size_t startLine = self->line;
	const size_t startColumn = self->column;
	if (self->position >= self->source.length)
	{
		return (Token) { TOKEN_EOF, ConstCharSpan_SubSpan(&self->source, startPosition, 0), self->line, self->column, { } };
	}

	// Whitespace
	char c = Lexer_ReadChar(self);
	if (c == ' ' || c == '\t' || c == '\n')
	{
		while (((c = Lexer_PeekChar(self))) && (c == ' ' || c == '\t' || c == '\n'))
			Lexer_ReadChar(self);

		if (includeWhitespace)
		{
			return (Token) {
				TOKEN_WHITESPACE,
				ConstCharSpan_SubSpan(&self->source, startPosition, self->position - startPosition),
				self->line,
				self->column,
				{ }
			};
		}

		goto restart;
	}

	char buf3[3];
	buf3[0] = c;
	buf3[1] = self->position < self->source.length ? self->source.data[self->position] : '\0';
	buf3[2] = self->position + 1 < self->source.length ? self->source.data[self->position + 1] : '\0';

	// Comments
	if (buf3[0] == '/' && buf3[1] == '/')
	{
		// Single-line comment
		Lexer_ReadChar(self); // Consume second '/'

		while (((c = Lexer_PeekChar(self))) && c != '\n')
			Lexer_ReadChar(self);

		if (includeComments)
		{
			return (Token) {
				TOKEN_COMMENT_SINGLELINE,
				ConstCharSpan_SubSpan(&self->source, startPosition, self->position - startPosition), startLine, startColumn,
				{ }
			};
		}

		goto restart;
	}

	if (buf3[0] == '/' && buf3[1] == '*')
	{
		// Multi-line comment
		Lexer_ReadChar(self); // Consume '*'

		while (true)
		{
			c = Lexer_ReadChar(self);
			if (c == '\0')
				break;

			if (c == '*' && Lexer_PeekChar(self) == '/')
			{
				Lexer_ReadChar(self); // consume '/'
				break;
			}
		}

		if (includeComments)
		{
			return (Token) {
				TOKEN_COMMENT_MULTILINE,
				ConstCharSpan_SubSpan(&self->source, startPosition, self->position - startPosition),
				startLine,
				startColumn,
				{ }
			};
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
			Lexer_ReadChar(self);
		}

		// Consume integer part, if any
		if (hasIntegerPart)
		{
			while (((c = Lexer_PeekChar(self))) && ((isDecimalOrOctal && IsDigit(c)) || (isBin && IsBinaryDigit(c)) || (isHex && IsHexDigit(c))))
			{
				if (c > '7' && isDecimalOrOctal)
					isValidOctal = false;
				Lexer_ReadChar(self);
			}
		}
		const ConstCharSpan integerPart = ConstCharSpan_SubSpan(&self->source, intStart, self->position - intStart);

		// Check for fractional part or exponent, to distinguish integer vs floating-point literal
		char nextCharLower = (char)tolower(Lexer_PeekChar(self));
		const bool isInteger = hasIntegerPart && nextCharLower != '.' && nextCharLower != FLOATING_POINT_EXONENT_CHAR(isHex);

		// Invalid (floating-point literal cannot be binary)
		if (!isInteger && isBin)
			abort(); // TODO: handle invalid binary floating-point literal

		// Integer literal
		if (isInteger)
		{
			// Read suffix
			buf3[0] = (char)tolower(Lexer_PeekChar(self));
			buf3[1] = self->position + 1 < self->source.length ? (char)tolower(self->source.data[self->position + 1]) : '\0';
			buf3[2] = self->position + 2 < self->source.length ? (char)tolower(self->source.data[self->position + 2]) : '\0';

			// Determine type based on suffix
			Token_LiteralInteger_Type type = TOKEN_LITERAL_INTEGER_TYPE_INT;
			if ((buf3[0] == 'u' && buf3[1] == 'l' && buf3[2] == 'l') || (buf3[0] == 'l' && buf3[1] == 'l' && buf3[2] == 'u')) // ULL / LLU
			{
				type = TOKEN_LITERAL_INTEGER_TYPE_UNSIGNEDLONGLONG;
				Lexer_ReadChar(self);
				Lexer_ReadChar(self);
				Lexer_ReadChar(self);
			}
			else if (buf3[0] == 'l' && buf3[1] == 'l') // LL / ll
			{
				type = TOKEN_LITERAL_INTEGER_TYPE_LONGLONG;
				Lexer_ReadChar(self);
				Lexer_ReadChar(self);
			}
			else if (buf3[0] == 'u' && buf3[1] == 'l') // UL
			{
				type = TOKEN_LITERAL_INTEGER_TYPE_UNSIGNEDLONG;
				Lexer_ReadChar(self);
				Lexer_ReadChar(self);
			}
			else if (buf3[0] == 'l') // L / l
			{
				type = TOKEN_LITERAL_INTEGER_TYPE_LONG;
				Lexer_ReadChar(self);
			}
			else if (buf3[0] == 'u') // U / u
			{
				type = TOKEN_LITERAL_INTEGER_TYPE_UNSIGNEDINT;
				Lexer_ReadChar(self);
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
				abort(); // TODO: handle invalid octal literal

			const ConstCharSpan lexeme = ConstCharSpan_SubSpan(&self->source, startPosition, self->position - startPosition);
			return (Token) { TOKEN_LITERAL_INTEGER, lexeme, startLine, startColumn, { .literalInteger = { integerPart, base, type } } };
		}

		// Floating-point literal

		// Consume fractional part (if any)
		bool hasFractionalPart = false;
		ConstCharSpan fractionalPart = ConstCharSpan_Empty;
		if (!hasIntegerPart || nextCharLower == '.')
		{
			// Consume '.' (only consume if there is an integer part)
			if (hasIntegerPart)
				Lexer_ReadChar(self);

			// Consume fractional digits
			const size_t fracStart = self->position;
			while (((c = Lexer_PeekChar(self))) && IsHexDigit(c))
				Lexer_ReadChar(self);

			// Determine if there actually is a fractional part
			hasFractionalPart = self->position > fracStart;
			fractionalPart = ConstCharSpan_SubSpan(&self->source, fracStart, self->position - fracStart);
		}

		// Consume exponent part, if any
		bool hasExponent = false;
		bool exponentIsNegative = false;
		ConstCharSpan exponent = ConstCharSpan_Empty;
		nextCharLower = (char)tolower(Lexer_PeekChar(self));
		if (nextCharLower == FLOATING_POINT_EXONENT_CHAR(isHex))
		{
			Lexer_ReadChar(self); // Consume exponent character

			if (Lexer_PeekChar(self) == '+' || Lexer_PeekChar(self) == '-')
				exponentIsNegative = Lexer_ReadChar(self) == '-'; // Consume '+' or '-'

			const size_t expStart = self->position;
			while (((c = Lexer_PeekChar(self))) && IsDigit(c))
				Lexer_ReadChar(self);

			hasExponent = self->position > expStart;
			if (!hasExponent)
				abort(); // TODO: handle invalid exponent

			exponent = ConstCharSpan_SubSpan(&self->source, expStart, self->position - expStart);
		}

		Token_LiteralFloat_Type suffix = TOKEN_LITERAL_FLOAT_TYPE_DOUBLE;
		// Consume suffix
		char suffixChar = (char)tolower(Lexer_PeekChar(self));
		if (suffixChar == 'd')
		{
			Lexer_ReadChar(self);
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
					abort(); // TODO: handle invalid decimal float suffix
			}

			Lexer_ReadChar(self);
		}
		else if (suffixChar == 'f' || suffixChar == 'l')
		{
			Lexer_ReadChar(self);
			switch (suffixChar)
			{
				case 'f':
				{
					buf3[0] = (char)tolower(Lexer_PeekChar(self));
					buf3[1] = self->position + 1 < self->source.length ? (char)tolower(self->source.data[self->position + 1]) : '\0';
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
						Lexer_ReadChar(self);
						Lexer_ReadChar(self);
					}
					break;
				}
				case 'l':
					suffix = TOKEN_LITERAL_FLOAT_TYPE_LONGDOUBLE;
					break;
				default:
					abort(); // Unreachable
			}
		}

		const ConstCharSpan lexeme = ConstCharSpan_SubSpan(&self->source, startPosition, self->position - startPosition);
		return (Token) {
			TOKEN_LITERAL_FLOAT, lexeme, startLine, startColumn, {
				.literalDecimalFloat = {
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
			}
		};
	}

	// String literals
	if (c == '"')
	{
		while (true)
		{
			c = Lexer_ReadChar(self);

			if (c == '\0' || c == '\n')
				abort(); // TODO: handle unterminated string literal

			// Closing quote
			if (c == '"')
				break;

			// Escape sequence
			if (c == '\\')
				Lexer_ReadChar(self);
		}

		return (Token) {
			TOKEN_LITERAL_STRING,
			ConstCharSpan_SubSpan(&self->source, startPosition, self->position - startPosition),
			startLine,
			startColumn,
			{ }
		};
	}

	// Punctuators
	for (size_t i = 0; i < sizeof(punctuators) / sizeof(TokenStringMapEntry); i++)
	{
		const TokenStringMapEntry* entry = &punctuators[i];

		const size_t len = strlen(entry->str);
		if (strncmp(buf3, entry->str, len) == 0)
		{
			for (size_t k = 1; k < len; k++)
				Lexer_ReadChar(self);

			return (Token) {
				entry->type, ConstCharSpan_SubSpan(&self->source, startPosition, len),
				startLine,
				startColumn,
				{ }
			};
		}
	}

	// Identifiers and keywords
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
	{
		while (((c = Lexer_PeekChar(self))) && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_'))
			Lexer_ReadChar(self);

		const ConstCharSpan lexeme = ConstCharSpan_SubSpan(&self->source, startPosition, self->position - startPosition);

		for (size_t i = 0; i < sizeof(keywords) / sizeof(TokenStringMapEntry); i++)
		{
			const TokenStringMapEntry* entry = &keywords[i];

			if (strlen(entry->str) == lexeme.length && memcmp(lexeme.data, entry->str, lexeme.length) == 0)
				return (Token) {
					entry->type, lexeme, startLine, startColumn,
					{ }
				};
		}

		return (Token) { TOKEN_IDENTIFIER, lexeme, self->line, self->column, { } };
	}

	// Unexpected character
	return (Token) {
		TOKEN_UNEXPECTED,
		ConstCharSpan_SubSpan(&self->source, startPosition, 1),
		self->line,
		self->column,
		{ }
	};
}

const char* Lexer_GetTokenTypeName(const Token_Type type)
{
	return tokenTypeName[type];
}

char Lexer_PeekChar(const Lexer* self)
{
	if (self->position >= self->source.length)
		return '\0';

	const char c = self->source.data[self->position];

	if (c == '\r')
		return '\n';

	return c;
}

char Lexer_ReadChar(Lexer* self)
{
	if (self->position >= self->source.length)
		return '\0';

	const char c = self->source.data[self->position++];
	if (c == '\r')
	{
		if (self->position < self->source.length && self->source.data[self->position] == '\n')
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
