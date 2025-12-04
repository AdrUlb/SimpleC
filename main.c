#include "Lexer.h"
#include "SourceFile.h"
#include "Util/Array.h"
#include "Util/Managed.h"

#define LIST_TYPE TokenList
#define LIST_ELEMENT_TYPE Token
#include "Util/ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

static TokenList* TokenizeSource(const SourceFile* source, CompilerErrorList* errorList)
{
	TokenList* tokens = New(TokenList);

	Lexer lexer;
	Lexer_Init(&lexer, source, errorList);

	while (true)
	{
		const Token token = Lexer_GetNextToken(&lexer, false, false);

		if (token.type == TOKEN_EOF)
			break;

		TokenList_Append(tokens, token);
	}

	Lexer_Fini(&lexer);
	return tokens;
}

int main(int argc, char* argv[])
{
	using const SourceFile* source = New(SourceFile, "test.c");
	using CompilerErrorList* errorList = New(CompilerErrorList);
	using const TokenList* tokens = TokenizeSource(source, errorList);

	for (size_t i = 0; i < tokens->size; i++)
	{
		const Token token = tokens->data[i];
		printf("%s: Lexeme='%.*s', Line=%zu, Column=%zu\n",
		       Lexer_GetTokenTypeName(token.type),
		       Span_AsFormat(&token.location.snippet),
		       token.location.line,
		       token.location.column);

		if (token.type == TOKEN_LITERAL_STRING)
		{
			using const String* value = Token_LiteralString_GetValue(&token);
			printf("  -> Value: '%s'\n", String_AsCString(value));
		}
		else if (token.type == TOKEN_LITERAL_INTEGER)
		{
			printf("  -> Value: '%.*s'\n", Span_AsFormat(&token.data.literalInteger.value));
			printf("  -> Base: %zu\n", token.data.literalInteger.base);
			printf("  -> Type: ");
			switch (token.data.literalInteger.type)
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
		else if (token.type == TOKEN_LITERAL_FLOAT)
		{
			printf("  -> Hexadecimal: %s\n", token.data.literalDecimalFloat.isHex ? "true" : "false");

			if (token.data.literalDecimalFloat.hasIntegerPart)
				printf("  -> Integer Part: %.*s\n", Span_AsFormat(&token.data.literalDecimalFloat.integerPart));
			else
				printf("  -> Integer Part: <none>\n");
			if (token.data.literalDecimalFloat.hasFractionalPart)
				printf("  -> Fractional Part: %.*s\n", Span_AsFormat(&token.data.literalDecimalFloat.fractionalPart));
			else
				printf("  -> Fractional Part: <none>\n");
			if (token.data.literalDecimalFloat.hasExponent)
				printf("  -> Exponent Part: %c%.*s\n", token.data.literalDecimalFloat.exponentIsNegative ? '-' : '+',
				       Span_AsFormat(&token.data.literalDecimalFloat.exponentPart));
			else
				printf("  -> Exponent Part: <none>\n");

			printf("  -> Type: ");
			switch (token.data.literalDecimalFloat.type)
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

	for (size_t i = 0; i < errorList->size; i++)
	{
		const CompilerError* error = errorList->data + i;
		printf("%s:%zu:%zu: error: %s\n", error->location.sourceFile->path, error->location.line, error->location.column, error->message);
	}

	return 0;
}
