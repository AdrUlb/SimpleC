#include "Token.h"

#include "Lexer.h"
#include "Util/Managed.h"

String* Token_LiteralString_GetValue(const Token* token)
{
	const ConstCharSpan lexeme = token->location.snippet;

	assert(lexeme.data[0] == '\"');
	assert(lexeme.data[lexeme.length - 1] == '\"');

	String* str = New(String);

	String tempStr;
	String_Init(&tempStr);

	for (size_t i = 1; i < lexeme.length - 1; i++)
	{
		char c = lexeme.data[i];
		if (c == '\\')
		{
			c = lexeme.data[++i];
			switch (c)
			{
				case 'a': // Alert (bell)
					String_AppendChar(str, '\a');
					break;
				case 'b': // Backspace
					String_AppendChar(str, '\b');
					break;
				case 'e': // Escape
					String_AppendChar(str, 0x1B);
					break;
				case 'f': // Formfeed
					String_AppendChar(str, '\f');
					break;
				case 'n': // Newline
					String_AppendChar(str, '\n');
					break;
				case 'r': // Carriage return
					String_AppendChar(str, '\r');
					break;
				case 't': // Horizontal tab
					String_AppendChar(str, '\t');
					break;
				case 'v': // Vertical tab
					String_AppendChar(str, '\v');
					break;
				case '\\': // Backslash
					String_AppendChar(str, '\\');
					break;
				case '\'': // Single quote
					String_AppendChar(str, '\'');
					break;
				case '"': // Double quote
					String_AppendChar(str, '"');
					break;
				case '?': // Question mark
					String_AppendChar(str, '?');
					break;
				case 'x': // Hexadecimal escape sequence
				{
					if (i == lexeme.length - 1)
					{
						// TODO: error: incomplete hex escape sequence
						abort();
					}

					String_Resize(&tempStr, 0);
					while ((c = lexeme.data[i + 1]))
					{
						if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
						{
							String_AppendChar(&tempStr, c);
							i++;
							continue;
						}

						break;
					}

					const char hexValue = (char)strtol(String_AsCString(&tempStr), NULL, 16);
					String_AppendChar(str, hexValue);
					break;
				}
				case 'u': // Unicode code point (16-bit)
				case 'U': // Unicode code point (32-bit)
				{
					String_Resize(&tempStr, 0);

					const size_t expectedDigits = (c == 'u') ? 4 : 8;
					for (size_t j = 0; j < expectedDigits; j++)
					{
						c = lexeme.data[i + 1];
						if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
						{
							String_AppendChar(&tempStr, c);
							i++;
						}
						else
						{
							break;
						}
					}

					const uint32_t codepoint = (uint32_t)strtol(String_AsCString(&tempStr), NULL, 16);
					String_AppendCodePoint(str, codepoint);
					break;
				}
				// Octal escape sequence
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				{
					String_Resize(&tempStr, 0);
					String_AppendChar(&tempStr, c);
					for (size_t j = 0; j < 2; j++)
					{
						c = lexeme.data[i + 1];
						if (c >= '0' && c <= '7')
						{
							String_AppendChar(&tempStr, c);
							i++;
						}
						else
						{
							break;
						}
					}

					const char octalValue = (char)strtol(String_AsCString(&tempStr), NULL, 8);
					String_AppendChar(str, octalValue);
					break;
				}
				default:
					// Unknown escape sequence, treat literally
					// NOTE: technically this is an error, let's be lenient
					String_AppendChar(str, c);
					break;
			}
		}
		else
		{
			String_AppendChar(str, c);
		}
	}

	String_Fini(&tempStr);
	return str;
}

void Token_Print(const Token* token)
{
	printf("%s: Lexeme='%.*s', Line=%zu, Column=%zu\n",
	       Token_Type_ToString(token->type),
	       Span_AsFormat(&token->location.snippet),
	       token->location.line,
	       token->location.column);

	if (token->type == TOKEN_LITERAL_STRING)
	{
		using const String* value = Token_LiteralString_GetValue(token);
		printf("  -> Value: '%s'\n", String_AsCString(value));
	}
	else if (token->type == TOKEN_LITERAL_INTEGER)
	{
		printf("  -> Value: '%.*s'\n", Span_AsFormat(&token->data.literalInteger.value));
		printf("  -> Base: %zu\n", token->data.literalInteger.base);
		printf("  -> Type: ");
		switch (token->data.literalInteger.type)
		{
			case TOKEN_LITERAL_INTEGER_TYPE_INT:
				printf("int\n");
				break;
			case TOKEN_LITERAL_INTEGER_TYPE_LONG:
				printf("long\n");
				break;
			case TOKEN_LITERAL_INTEGER_TYPE_LONGLONG:
				printf("long long\n");
				break;
			case TOKEN_LITERAL_INTEGER_TYPE_UNSIGNEDINT:
				printf("unsigned int\n");
				break;
			case TOKEN_LITERAL_INTEGER_TYPE_UNSIGNEDLONG:
				printf("unsigned long\n");
				break;
			case TOKEN_LITERAL_INTEGER_TYPE_UNSIGNEDLONGLONG:
				printf("unsigned long long\n");
				break;
		}
	}
	else if (token->type == TOKEN_LITERAL_FLOAT)
	{
		printf("  -> Hexadecimal: %s\n", token->data.literalDecimalFloat.isHex ? "true" : "false");

		if (token->data.literalDecimalFloat.hasIntegerPart)
			printf("  -> Integer Part: %.*s\n", Span_AsFormat(&token->data.literalDecimalFloat.integerPart));
		else
			printf("  -> Integer Part: <none>\n");
		if (token->data.literalDecimalFloat.hasFractionalPart)
			printf("  -> Fractional Part: %.*s\n", Span_AsFormat(&token->data.literalDecimalFloat.fractionalPart));
		else
			printf("  -> Fractional Part: <none>\n");
		if (token->data.literalDecimalFloat.hasExponent)
			printf("  -> Exponent Part: %c%.*s\n", token->data.literalDecimalFloat.exponentIsNegative ? '-' : '+',
			       Span_AsFormat(&token->data.literalDecimalFloat.exponentPart));
		else
			printf("  -> Exponent Part: <none>\n");

		printf("  -> Type: ");
		switch (token->data.literalDecimalFloat.type)
		{
			case TOKEN_LITERAL_FLOAT_TYPE_DOUBLE:
				printf("double\n");
				break;
			case TOKEN_LITERAL_FLOAT_TYPE_FLOAT:
				printf("float\n");
				break;
			case TOKEN_LITERAL_FLOAT_TYPE_LONGDOUBLE:
				printf("long double\n");
				break;
			case TOKEN_LITERAL_FLOAT_TYPE_BINARY16:
				printf("binary16\n");
				break;
			case TOKEN_LITERAL_FLOAT_TYPE_BINARY32:
				printf("binary32\n");
				break;
			case TOKEN_LITERAL_FLOAT_TYPE_BINARY64:
				printf("binary64\n");
				break;
			case TOKEN_LITERAL_FLOAT_TYPE_DECIMAL32:
				printf("decimal32\n");
				break;
			case TOKEN_LITERAL_FLOAT_TYPE_DECIMAL64:
				printf("decimal64\n");
				break;
			case TOKEN_LITERAL_FLOAT_TYPE_DECIMAL128:
				printf("decimal128\n");
				break;
		}
	}
}
