#include "Lexer.h"
#include "Parser.h"
#include "SourceFile.h"
#include "Util/Array.h"
#include "Util/Managed.h"

typedef struct
{
	size_t indent;
	String output;
} AstPrinter;

static AstPrinter AstPrinter_Create(void)
{
	AstPrinter printer;
	printer.indent = 0;
	String_Init(&printer.output);
	return printer;
}

static void AstPrinter_Fini(const AstPrinter* self)
{
	String_Fini(&self->output);
}

static void AstPrinter_PrintIndentation(AstPrinter* self)
{
	for (size_t i = 0; i < self->indent * 4; i++)
		String_AppendChar(&self->output, ' ');
}

typedef int a;

static void AstPrinter_PrintTypeName(AstPrinter* self, const AstTypeName* type)
{
	const AstTypeQualifiers qualifiers = type->specifierQualifierList->qualifiers;
	using const String* str = AstTypeQualifiers_ToString(qualifiers);
	String_AppendCString(&self->output, String_AsCString(str));

	for (size_t i = 0; i < type->specifierQualifierList->specifiers->size; i++)
	{
		const AstTypeSpecifier* specifier = type->specifierQualifierList->specifiers->data[i];
		String_AppendCString(&self->output, AstTypeSpecifier_Type_ToString(specifier->type));
		if (i < type->specifierQualifierList->specifiers->size - 1)
			String_AppendChar(&self->output, ' ');
	}
}

static void AstPrinter_PrintExpression(AstPrinter* self, const AstExpression* expr)
{
	switch (expr->type)
	{
		case AST_EXPR_UNARY:
			String_AppendCString(&self->output, "(Unary Expression) {\n");
			self->indent++;

			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "Operation: ");
			String_AppendCString(&self->output, AstUnaryOperation_ToString(expr->data.unary.operation));
			String_AppendCString(&self->output, ",\n");

			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "Expression: ");
			AstPrinter_PrintExpression(self, expr->data.unary.expression);
			String_AppendCString(&self->output, "\n");

			self->indent--;
			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "}");
			break;
		case AST_EXPR_BINARY:
			String_AppendCString(&self->output, "(Binary Expression) {\n");
			self->indent++;

			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "Operation: ");
			String_AppendCString(&self->output, AstBinaryOperation_ToString(expr->data.binary.operation));
			String_AppendCString(&self->output, ",\n");

			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "Left: ");
			AstPrinter_PrintExpression(self, expr->data.binary.left);
			String_AppendCString(&self->output, ",\n");

			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "Right: ");
			AstPrinter_PrintExpression(self, expr->data.binary.right);
			String_AppendChar(&self->output, '\n');

			self->indent--;
			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "}");
			break;
		case AST_EXPR_TERNARY:
			String_AppendCString(&self->output, "(Ternary Expression) {\n");
			self->indent++;

			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "Operation: ");
			String_AppendCString(&self->output, AstTernaryOperation_ToString(expr->data.ternary.operation));
			String_AppendCString(&self->output, ",\n");

			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "Left: ");
			AstPrinter_PrintExpression(self, expr->data.ternary.left);
			String_AppendCString(&self->output, ",\n");

			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "Middle: ");
			AstPrinter_PrintExpression(self, expr->data.ternary.middle);
			String_AppendCString(&self->output, ",\n");

			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "Right: ");
			AstPrinter_PrintExpression(self, expr->data.ternary.right);
			String_AppendChar(&self->output, '\n');

			self->indent--;
			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "}");
			break;
		case AST_EXPR_CAST:
			String_AppendCString(&self->output, "(Cast Expression) {\n");
			self->indent++;

			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "TODO!\n");

			self->indent--;
			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "}");
			break;
		case AST_EXPR_SIZEOF_TYPE:
			String_AppendCString(&self->output, "(Sizeof Type Expression) {\n");
			self->indent++;

			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "Type: ");
			AstPrinter_PrintTypeName(self, expr->data.sizeofType.typeName);
			String_AppendChar(&self->output, '\n');

			self->indent--;
			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "}");
			break;
		case AST_EXPR_MEMBER_ACCESS:
			String_AppendCString(&self->output, "(Member Access Expression) {\n");
			self->indent++;

			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "TODO!\n");

			self->indent--;
			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "}");
			break;
		case AST_EXPR_CALL:
			String_AppendCString(&self->output, "(Call Expression) {\n");
			self->indent++;

			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "Arguments: [\n");
			self->indent++;
			for (size_t i = 0; i < expr->data.call.arguments->size; i++)
			{
				AstPrinter_PrintIndentation(self);
				AstPrinter_PrintExpression(self, expr->data.call.arguments->data[i]);
				if (i < expr->data.call.arguments->size - 1)
					String_AppendChar(&self->output, ',');
				String_AppendChar(&self->output, '\n');
			}
			self->indent--;
			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "]\n");

			self->indent--;
			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "}");
			break;
		case AST_EXPR_PRIMARY:
		{
			String_AppendCString(&self->output, "(Primary Expression) {\n");
			self->indent++;

			AstPrinter_PrintIndentation(self);
			const Token* token = &expr->data.primary.literal;
			switch (token->type)
			{
				case TOKEN_LITERAL_INTEGER:
					String_AppendCString(&self->output, "Type: Integer Literal { Value: ");
					String_AppendConstCharSpan(&self->output, token->location.snippet);
					String_AppendCString(&self->output, ", ");
					String_AppendCString(&self->output, "Type: ");
					String_AppendCString(&self->output, token->data.literalInteger.type == TOKEN_LITERAL_INTEGER_TYPE_INT
						                                    ? "int"
						                                    : token->data.literalInteger.type == TOKEN_LITERAL_INTEGER_TYPE_LONG
							                                      ? "long"
							                                      : token->data.literalInteger.type == TOKEN_LITERAL_INTEGER_TYPE_LONGLONG
								                                        ? "long long"
								                                        : token->data.literalInteger.type == TOKEN_LITERAL_INTEGER_TYPE_UNSIGNEDINT
									                                          ? "unsigned int"
									                                          : token->data.literalInteger.type == TOKEN_LITERAL_INTEGER_TYPE_UNSIGNEDLONG
										                                            ? "unsigned long"
										                                            : token->data.literalInteger.type ==
										                                              TOKEN_LITERAL_INTEGER_TYPE_UNSIGNEDLONGLONG
											                                              ? "unsigned long long"
											                                              : "???");
					String_AppendCString(&self->output, ", ");
					String_AppendCString(&self->output, "Base: ");
					String_AppendCString(&self->output, token->data.literalInteger.base == 10
						                                    ? "10"
						                                    : token->data.literalInteger.base == 16
							                                      ? "16"
							                                      : token->data.literalInteger.base == 8
								                                        ? "8"
								                                        : token->data.literalInteger.base == 2
									                                          ? "2"
									                                          : "???");
					String_AppendCString(&self->output, " }\n");
					break;
				case TOKEN_LITERAL_FLOAT:
					String_AppendCString(&self->output, "Type: Floating Point Literal { Value: ");
					String_AppendConstCharSpan(&self->output, token->location.snippet);
					String_AppendCString(&self->output, " }\n");
					break;
				case TOKEN_LITERAL_CHAR:
					String_AppendCString(&self->output, "Type: Character Literal { Value: ");
					String_AppendConstCharSpan(&self->output, token->location.snippet);
					String_AppendCString(&self->output, " }\n");
					break;
				case TOKEN_LITERAL_STRING:
					String_AppendCString(&self->output, "Type: String Literal { Value: ");
					String_AppendConstCharSpan(&self->output, token->location.snippet);
					String_AppendCString(&self->output, " }\n");
					break;
				case TOKEN_IDENTIFIER:
					String_AppendCString(&self->output, "Type: Identifier { Name: ");
					String_AppendConstCharSpan(&self->output, token->location.snippet);
					String_AppendCString(&self->output, " }\n");
					break;
				default:
					abort(); // Unexpected token type
			}

			self->indent--;
			AstPrinter_PrintIndentation(self);
			String_AppendCString(&self->output, "}");
			break;
		}
	}
}

static void Parse(const SourceFile* source, CompilerErrorList* errorList)
{
	using TokenList* tokens = New(TokenList);

	Lexer lexer = Lexer_Create(source, errorList);

	while (true)
	{
		const Token token = Lexer_GetNextToken(&lexer, false, false);

		TokenList_AppendFromPtr(tokens, &token);

		if (token.type == TOKEN_EOF)
			break;
	}

	for (size_t i = 0; i < tokens->size; i++)
	{
		const Token* token = &tokens->data[i];
		Token_Print(token);
	}

	Parser parser = Parser_Create(tokens, errorList);

	using const AstExpression* expr = Parser_ParseExpression(&parser);
	if (expr)
	{
		AstPrinter printer = AstPrinter_Create();
		AstPrinter_PrintExpression(&printer, expr);
		printf("%s\n", String_AsCString(&printer.output));
		AstPrinter_Fini(&printer);
	}
}

int main(const int argc, char* argv[])
{
	const CStringSpan args = CStringSpan_Create(argv, argc);

	using const SourceFile* source = NewWith(SourceFile, Path, "tests/expression_test4.c");
	if (source == NULL)
	{
		printf("Failed to open source file: tests/expression_test.c\n");
		return 1;
	}

	using CompilerErrorList* errorList = New(CompilerErrorList);
	Parse(source, errorList);

	for (size_t i = 0; i < errorList->size; i++)
	{
		const CompilerError* error = errorList->data + i;
		printf("%s:%zu:%zu: error: %s\n", error->location.sourceFile->path, error->location.line, error->location.column, error->message);
	}

	return 0;
}
