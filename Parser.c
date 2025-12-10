// https://learn.microsoft.com/en-us/cpp/c-language/phrase-structure-grammar

#include "Parser.h"

#include <math.h>
#include "AstDeclaration.h"
#include "AstDeclarationSpecifiers.h"
#include "AstDeclarator.h"
#include "AstDirectDeclarator.h"
#include "AstFunctionSpecifier.h"
#include "AstPointer.h"
#include "AstStorageClassSpecifier.h"
#include "Util/Managed.h"

static Token* Parser_PeekToken(const Parser* self);
static Token* Parser_ConsumeToken(Parser* self);
static bool Parser_MatchToken(Parser* self, Token_Type type, SourceLocation* outLocation);

static AstExpression* Parser_ParsePrimaryExpression(Parser* self);
static AstExpression* Parser_ParsePostfixExpression(Parser* self);
static AstExpression* Parser_ParseUnaryExpression(Parser* self);
static AstExpression* Parser_ParseCastExpression(Parser* self);
static AstExpression* Parser_ParseMultiplicativeExpression(Parser* self);
static AstExpression* Parser_ParseAdditiveExpression(Parser* self);
static AstExpression* Parser_ParseShiftExpression(Parser* self);
static AstExpression* Parser_ParseRelationalExpression(Parser* self);
static AstExpression* Parser_ParseEqualityExpression(Parser* self);
static AstExpression* Parser_ParseBitwiseAndExpression(Parser* self);
static AstExpression* Parser_ParseBitwiseXorExpression(Parser* self);
static AstExpression* Parser_ParseBitwiseOrExpression(Parser* self);
static AstExpression* Parser_ParseLogicalAndExpression(Parser* self);
static AstExpression* Parser_ParseLogicalOrExpression(Parser* self);
static AstExpression* Parser_ParseConditionalExpression(Parser* self);
static AstExpression* Parser_ParseAssignmentExpression(Parser* self);
static AstDeclaration* Parser_TryParseDeclaration(Parser* self);
static AstDeclarationSpecifiers* Parser_ParseDeclarationSpecifiers(Parser* self);
static AstStorageClassSpecifier* Parser_TryParseStorageClassSpecifier(Parser* self);
static AstTypeSpecifier* Parser_TryParseTypeSpecifier(Parser* self);
static AstTypeSpecifierQualifierList* Parser_TryParseSpecifierQualifierList(Parser* self);
static AstTypeQualifiers Parser_TryParseTypeQualifier(Parser* self, SourceLocation* outLocation);
static AstFunctionSpecifiers Parser_TryParseFunctionSpecifier(Parser* self, SourceLocation* outLocation);
static AstDeclarator* Parser_TryParseDeclarator(Parser* self);
static AstDirectDeclarator* Parser_TryParseDirectDeclarator(Parser* self);
static AstPointer* Parser_TryParsePointer(Parser* self);
static AstTypeQualifiers Parser_TryParseTypeQualifierList(Parser* self, SourceLocation* outLocation);
static AstTypeName* Parser_TryParseTypeName(Parser* self);

Token* Parser_PeekToken(const Parser* self)
{
	if (self->currentTokenIndex >= self->tokens->size)
		return &self->tokens->data[self->tokens->size - 1]; // Return EOF token

	return &self->tokens->data[self->currentTokenIndex];
}

Token* Parser_ConsumeToken(Parser* self)
{
	if (self->currentTokenIndex >= self->tokens->size)
		return NULL;

	return &self->tokens->data[self->currentTokenIndex++];
}

bool Parser_MatchToken(Parser* self, const Token_Type type, SourceLocation* outLocation)
{
	const Token* token = Parser_PeekToken(self);
	if (token == NULL || token->type != type)
		return false;

	Parser_ConsumeToken(self);
	if (outLocation != NULL)
		*outLocation = token->location;
	return true;
}

AstExpression* Parser_ParsePrimaryExpression(Parser* self)
{
	const Token* token = Parser_PeekToken(self);
	if (token == NULL)
		return NULL;

	if (token->type == TOKEN_PUNCTUATOR_PARENOPEN)
	{
		// (<expression>)
		Parser_ConsumeToken(self);

		AstExpression* expr = Parser_ParseExpression(self);
		if (!expr)
			return NULL;

		if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENCLOSE, NULL))
		{
			CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_ExpectedClosingParenthesisInParenthesizedExpression, expr->location));
			return NULL;
		}

		return expr;
	}

	if (token->type == TOKEN_LITERAL_INTEGER || // integer-constant
	    token->type == TOKEN_LITERAL_FLOAT || // floating-constant
	    token->type == TOKEN_LITERAL_CHAR || // character-constant
	    token->type == TOKEN_LITERAL_STRING || // string-literal
	    token->type == TOKEN_IDENTIFIER) // identifier or constant (enumeration-constant)
	{
		Parser_ConsumeToken(self);

		return NewWith(AstExpression, Primary,
		               (AstPrimaryExpression){ *token },
		               token->location);
	}

	// TODO: generic-selection

	return NULL;
}

AstExpression* Parser_ParsePostfixExpression(Parser* self)
{
	const size_t savedTokenIndex = self->currentTokenIndex;

	AstExpression* expression = NULL;
	if (true)
	{
		if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENOPEN, NULL))
			goto primary;

		using const AstTypeName* type = Parser_TryParseTypeName(self);
		if (!type)
			goto primary;

		if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENCLOSE, NULL))
			goto primary;

		if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_BRACEOPEN, NULL))
			goto primary;

		// TODO: initializer list
		RetainConst(type);
		abort();
	}
	else
	{
	primary:
		self->currentTokenIndex = savedTokenIndex;
		expression = Parser_ParsePrimaryExpression(self);
	}

	if (!expression)
		return NULL;

	while (true)
	{
		const Token* token = Parser_PeekToken(self);
		if (token == NULL)
			break;

		if (token->type == TOKEN_PUNCTUATOR_BRACKETOPEN)
		{
			// Subscript expression
			Parser_ConsumeToken(self);
			AstExpression* indexExpr = Parser_ParseExpression(self);
			if (!indexExpr)
				return NULL;

			if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_BRACKETCLOSE, NULL))
			{
				CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_ExpectedClosingBracketInSubscriptExpression, indexExpr->location));
				return NULL;
			}

			expression = NewWith(AstExpression, Binary,
			                     (AstBinaryExpression){ AST_BINOP_SUBSCRIPT, expression, indexExpr },
			                     SourceLocation_Concat(&expression->location, &indexExpr->location));
			continue;
		}

		if (token->type == TOKEN_PUNCTUATOR_PERIOD || token->type == TOKEN_PUNCTUATOR_MINUS_GREATER)
		{
			// Member access
			Parser_ConsumeToken(self);

			const Token* indentifier = Parser_PeekToken(self);

			if (indentifier == NULL || indentifier->type != TOKEN_IDENTIFIER)
			{
				CompilerErrorList_Append(
					self->errors, CompilerError_Create("expected identifier after '.' or '->' in member access expression", token->location));
				return NULL;
			}

			Parser_ConsumeToken(self);

			expression = NewWith(AstExpression, MemberAccess,
			                     (AstMemberAccessExpression){ expression, indentifier->data.literalIdentifier.value,
			                     token->type == TOKEN_PUNCTUATOR_MINUS_GREATER },
			                     SourceLocation_Concat(&expression->location, &indentifier->location));
			continue;
		}

		if (token->type == TOKEN_PUNCTUATOR_PLUS_PLUS || token->type == TOKEN_PUNCTUATOR_MINUS_MINUS)
		{
			// Postfix increment/decrement
			Parser_ConsumeToken(self);

			const AstUnaryOperation op = (token->type == TOKEN_PUNCTUATOR_PLUS_PLUS) ? AST_UNOP_POST_INCREMENT : AST_UNOP_POST_DECREMENT;

			expression = NewWith(AstExpression, Unary,
			                     (AstUnaryExpression){ op, expression },
			                     SourceLocation_Concat(&expression->location, &token->location));
			continue;
		}

		if (token->type == TOKEN_PUNCTUATOR_PARENOPEN)
		{
			// Function call
			Parser_ConsumeToken(self);

			using AstExpressionList* args = New(AstExpressionList);
			while (true)
			{
				if (Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENCLOSE, NULL))
					break;

				AstExpression* argExpr = Parser_ParseAssignmentExpression(self);
				if (!argExpr)
					return NULL;

				AstExpressionList_Append(args, argExpr);

				if (Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENCLOSE, NULL))
					break;

				if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_COMMA, NULL))
				{
					CompilerErrorList_Append(self->errors, CompilerError_Create("expected ',' or ')' in function call argument list", argExpr->location));
					return NULL;
				}
			}

			expression = NewWith(AstExpression, Call,
			                     (AstCallExpression){ expression, Retain(args) },
			                     SourceLocation_Concat(&expression->location, &token->location));
			continue;
		}
		break;
	}

	return expression;
}

AstExpression* Parser_ParseUnaryExpression(Parser* self)
{
	const Token* token = Parser_PeekToken(self);
	if (token == NULL)
		return NULL;

	// "sizeof(<type>)" or "sizeof <expression>"
	if (token->type == TOKEN_KEYWORD_SIZEOF)
	{
		Parser_ConsumeToken(self);

		// sizeof(<type>)
		if (Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENOPEN, NULL))
		{
			AstTypeName* type = Parser_TryParseTypeName(self);
			if (!type)
				return NULL;

			if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENCLOSE, NULL))
			{
				CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_ExpectedClosingParenthesisInSizeofTypeExpression, type->location));
				Release(type);
				return NULL;
			}

			return NewWith(AstExpression, SizeofType,
			               (AstSizeofTypeExpression){ type },
			               SourceLocation_Concat(&token->location, &type->location));
		}

		// sizeof <expression>
		AstExpression* expr = Parser_ParseUnaryExpression(self);
		if (!expr)
			return NULL;

		return NewWith(AstExpression, Unary,
		               (AstUnaryExpression){ AST_UNOP_SIZEOF, expr },
		               SourceLocation_Concat(&token->location, &expr->location));
	}

	// Prefix expressions
	AstUnaryOperation op;
	switch (token->type)
	{
		case TOKEN_PUNCTUATOR_AMPERSAND:
			op = AST_UNOP_ADDRESS_OF;
			break;
		case TOKEN_PUNCTUATOR_ASTERISK:
			op = AST_UNOP_DEREFERENCE;
			break;
		case TOKEN_PUNCTUATOR_PLUS:
			op = AST_UNOP_PLUS;
			break;
		case TOKEN_PUNCTUATOR_MINUS:
			op = AST_UNOP_MINUS;
			break;
		case TOKEN_PUNCTUATOR_TILDE:
			op = AST_UNOP_BITWISE_NOT;
			break;
		case TOKEN_PUNCTUATOR_EXCLAMATION:
			op = AST_UNOP_LOGICAL_NOT;
			break;
		case TOKEN_PUNCTUATOR_PLUS_PLUS:
			op = AST_UNOP_PRE_INCREMENT;
			break;
		case TOKEN_PUNCTUATOR_MINUS_MINUS:
			op = AST_UNOP_PRE_DECREMENT;
			break;
		default:
			op = AST_UNOP_NONE;
	}

	if (op == AST_UNOP_PRE_INCREMENT || op == AST_UNOP_PRE_DECREMENT)
	{
		Parser_ConsumeToken(self);
		AstExpression* expr = Parser_ParseUnaryExpression(self);
		if (!expr)
			return NULL;

		return NewWith(AstExpression, Unary,
		               (AstUnaryExpression){ op, expr },
		               SourceLocation_Concat(&token->location, &expr->location));
	}

	if (op != AST_UNOP_NONE)
	{
		Parser_ConsumeToken(self);
		AstExpression* expr = Parser_ParseCastExpression(self);
		if (!expr)
			return NULL;

		return NewWith(AstExpression, Unary,
		               (AstUnaryExpression){ op, expr },
		               SourceLocation_Concat(&token->location, &expr->location));
	}

	return Parser_ParsePostfixExpression(self);
}

AstExpression* Parser_ParseCastExpression(Parser* self)
{
	const size_t savedTokenIndex = self->currentTokenIndex;

	SourceLocation startLocation;
	if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENOPEN, &startLocation))
		return Parser_ParseUnaryExpression(self);

	AstTypeName* type = Parser_TryParseTypeName(self);
	if (!type)
	{
		// Not a cast expression, rewind and parse as unary expression
		self->currentTokenIndex = savedTokenIndex;
		return Parser_ParseUnaryExpression(self);
	}

	if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENCLOSE, NULL))
	{
		CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_ExpectedClosingParenthesisInCastExpression, type->location));
		Release(type);
		return NULL;
	}

	AstExpression* expr = Parser_ParseCastExpression(self);
	if (!expr)
	{
		Release(type);
		return NULL;
	}
	return NewWith(AstExpression, Cast, (AstCastExpression){ type, expr }, SourceLocation_Concat(&startLocation, &expr->location));
}

AstExpression* Parser_ParseMultiplicativeExpression(Parser* self)
{
	AstExpression* lhs = Parser_ParseCastExpression(self);
	if (!lhs)
		return NULL;

	while (true)
	{
		const Token* token = Parser_PeekToken(self);
		if (token == NULL)
			break;

		AstBinaryOperation op;
		switch (token->type)
		{
			case TOKEN_PUNCTUATOR_ASTERISK:
				op = AST_BINOP_MULTIPLY;
				break;
			case TOKEN_PUNCTUATOR_SLASH:
				op = AST_BINOP_DIVIDE;
				break;
			case TOKEN_PUNCTUATOR_PERCENT:
				op = AST_BINOP_MODULO;
				break;
			default:
				op = AST_BINOP_NONE;
		}

		if (op == AST_BINOP_NONE)
			break;

		Parser_ConsumeToken(self);
		AstExpression* rhs = Parser_ParseCastExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(AstExpression, Binary, (AstBinaryExpression){ op, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

AstExpression* Parser_ParseAdditiveExpression(Parser* self)
{
	AstExpression* lhs = Parser_ParseMultiplicativeExpression(self);
	if (!lhs)
		return NULL;

	while (true)
	{
		const Token* token = Parser_PeekToken(self);
		if (token == NULL)
			break;

		AstBinaryOperation op;
		switch (token->type)
		{
			case TOKEN_PUNCTUATOR_PLUS:
				op = AST_BINOP_ADD;
				break;
			case TOKEN_PUNCTUATOR_MINUS:
				op = AST_BINOP_SUBTRACT;
				break;
			default:
				op = AST_BINOP_NONE;
		}

		if (op == AST_BINOP_NONE)
			break;

		Parser_ConsumeToken(self);
		AstExpression* rhs = Parser_ParseMultiplicativeExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(AstExpression, Binary, (AstBinaryExpression){ op, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

AstExpression* Parser_ParseShiftExpression(Parser* self)
{
	AstExpression* lhs = Parser_ParseAdditiveExpression(self);
	if (!lhs)
		return NULL;

	while (true)
	{
		const Token* token = Parser_PeekToken(self);
		if (token == NULL)
			break;

		AstBinaryOperation op;
		switch (token->type)
		{
			case TOKEN_PUNCTUATOR_LESS_LESS:
				op = AST_BINOP_SHIFT_LEFT;
				break;
			case TOKEN_PUNCTUATOR_GREATER_GREATER:
				op = AST_BINOP_SHIFT_RIGHT;
				break;
			default:
				op = AST_BINOP_NONE;
		}

		if (op == AST_BINOP_NONE)
			break;

		Parser_ConsumeToken(self);
		AstExpression* rhs = Parser_ParseAdditiveExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(AstExpression, Binary, (AstBinaryExpression){ op, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

AstExpression* Parser_ParseRelationalExpression(Parser* self)
{
	AstExpression* lhs = Parser_ParseShiftExpression(self);
	if (!lhs)
		return NULL;

	while (true)
	{
		const Token* token = Parser_PeekToken(self);
		if (token == NULL)
			break;

		AstBinaryOperation op;
		switch (token->type)
		{
			case TOKEN_PUNCTUATOR_LESS:
				op = AST_BINOP_TEST_LESS;
				break;
			case TOKEN_PUNCTUATOR_GREATER:
				op = AST_BINOP_TEST_GREATER;
				break;
			case TOKEN_PUNCTUATOR_LESS_EQUAL:
				op = AST_BINOP_TEST_LESS_EQUAL;
				break;
			case TOKEN_PUNCTUATOR_GREATER_EQUAL:
				op = AST_BINOP_TEST_GREATER_EQUAL;
				break;
			default:
				op = AST_BINOP_NONE;
		}

		if (op == AST_BINOP_NONE)
			break;

		Parser_ConsumeToken(self);
		AstExpression* rhs = Parser_ParseShiftExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(AstExpression, Binary, (AstBinaryExpression){ op, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

AstExpression* Parser_ParseEqualityExpression(Parser* self)
{
	AstExpression* lhs = Parser_ParseRelationalExpression(self);
	if (!lhs)
		return NULL;

	while (true)
	{
		const Token* token = Parser_PeekToken(self);
		if (token == NULL)
			break;

		AstBinaryOperation op;
		switch (token->type)
		{
			case TOKEN_PUNCTUATOR_EQUAL_EQUAL:
				op = AST_BINOP_TEST_EQUAL;
				break;
			case TOKEN_PUNCTUATOR_EXCLAMATION_EQUAL:
				op = AST_BINOP_TEST_NOT_EQUAL;
				break;
			default:
				op = AST_BINOP_NONE;
		}

		if (op == AST_BINOP_NONE)
			break;

		Parser_ConsumeToken(self);
		AstExpression* rhs = Parser_ParseRelationalExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(AstExpression, Binary, (AstBinaryExpression){ op, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

AstExpression* Parser_ParseBitwiseAndExpression(Parser* self)
{
	AstExpression* lhs = Parser_ParseEqualityExpression(self);
	if (!lhs)
		return NULL;

	while (Parser_MatchToken(self, TOKEN_PUNCTUATOR_AMPERSAND, NULL))
	{
		AstExpression* rhs = Parser_ParseEqualityExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(AstExpression, Binary, (AstBinaryExpression){ AST_BINOP_BITWISE_AND, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

AstExpression* Parser_ParseBitwiseXorExpression(Parser* self)
{
	AstExpression* lhs = Parser_ParseBitwiseAndExpression(self);
	if (!lhs)
		return NULL;

	while (Parser_MatchToken(self, TOKEN_PUNCTUATOR_CARET, NULL))
	{
		AstExpression* rhs = Parser_ParseBitwiseAndExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(AstExpression, Binary, (AstBinaryExpression){ AST_BINOP_BITWISE_XOR, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

AstExpression* Parser_ParseBitwiseOrExpression(Parser* self)
{
	AstExpression* lhs = Parser_ParseBitwiseXorExpression(self);
	if (!lhs)
		return NULL;

	while (Parser_MatchToken(self, TOKEN_PUNCTUATOR_PIPE, NULL))
	{
		AstExpression* rhs = Parser_ParseBitwiseXorExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(AstExpression, Binary, (AstBinaryExpression){ AST_BINOP_BITWISE_OR, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

AstExpression* Parser_ParseLogicalAndExpression(Parser* self)
{
	AstExpression* lhs = Parser_ParseBitwiseOrExpression(self);
	if (!lhs)
		return NULL;

	while (Parser_MatchToken(self, TOKEN_PUNCTUATOR_AMPERSAND_AMPERSAND, NULL))
	{
		AstExpression* rhs = Parser_ParseBitwiseOrExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(AstExpression, Binary, (AstBinaryExpression){ AST_BINOP_LOGICAL_AND, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

AstExpression* Parser_ParseLogicalOrExpression(Parser* self)
{
	AstExpression* lhs = Parser_ParseLogicalAndExpression(self);
	if (!lhs)
		return NULL;

	while (Parser_MatchToken(self, TOKEN_PUNCTUATOR_PIPE_PIPE, NULL))
	{
		AstExpression* rhs = Parser_ParseLogicalAndExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(AstExpression, Binary, (AstBinaryExpression){ AST_BINOP_LOGICAL_OR, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

AstExpression* Parser_ParseConditionalExpression(Parser* self)
{
	AstExpression* lhs = Parser_ParseLogicalOrExpression(self);
	if (!lhs)
		return NULL;

	while (Parser_MatchToken(self, TOKEN_PUNCTUATOR_QUESTION, NULL))
	{
		AstExpression* ifTrue = Parser_ParseExpression(self);
		if (!ifTrue)
			break;

		if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_COLON, NULL))
		{
			CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_ExpectedColonInConditionalExpression, lhs->location));
			break;
		}

		AstExpression* ifFalse = Parser_ParseConditionalExpression(self);
		if (!ifFalse)
			break;

		lhs = NewWith(AstExpression, Ternary,
		              (AstTernaryExpression){ AST_TERNOP_CONDITIONAL, lhs, ifTrue, ifFalse },
		              SourceLocation_Concat(&lhs->location, &ifFalse->location));
	}

	return lhs;
}

AstExpression* Parser_ParseAssignmentExpression(Parser* self)
{
	// NOTE: Technically, according to the C standard, this is either
	//  - a unary expression or
	//  - a conditional expression followed by an assignment operator and another assignment expression.
	// This definition enforces that only l-values can be assigned to within the language grammar.
	// However, for simplicity, we will parse it as a conditional expression followed by an optional
	// assignment operator and another assignment expression, and perform l-value checking later during semantic analysis.
	AstExpression* lhs = Parser_ParseConditionalExpression(self);
	if (!lhs)
		return NULL;

	const Token* token = Parser_PeekToken(self);
	if (token != NULL)
	{
		AstBinaryOperation op;
		switch (token->type)
		{
			case TOKEN_PUNCTUATOR_EQUAL:
				op = AST_BINOP_ASSIGN;
				break;
			case TOKEN_PUNCTUATOR_ASTERISK_EQUAL:
				op = AST_BINOP_ASSIGN_MULTIPLY;
				break;
			case TOKEN_PUNCTUATOR_SLASH_EQUAL:
				op = AST_BINOP_ASSIGN_DIVIDE;
				break;
			case TOKEN_PUNCTUATOR_PERCENT_EQUAL:
				op = AST_BINOP_ASSIGN_MODULO;
				break;
			case TOKEN_PUNCTUATOR_PLUS_EQUAL:
				op = AST_BINOP_ASSIGN_ADD;
				break;
			case TOKEN_PUNCTUATOR_MINUS_EQUAL:
				op = AST_BINOP_ASSIGN_SUBTRACT;
				break;
			case TOKEN_PUNCTUATOR_LESS_LESS_EQUAL:
				op = AST_BINOP_ASSIGN_LEFT_SHIFT;
				break;
			case TOKEN_PUNCTUATOR_GREATER_GREATER_EQUAL:
				op = AST_BINOP_ASSIGN_RIGHT_SHIFT;
				break;
			case TOKEN_PUNCTUATOR_AMPERSAND_EQUAL:
				op = AST_BINOP_ASSIGN_AND;
				break;
			case TOKEN_PUNCTUATOR_CARET_EQUAL:
				op = AST_BINOP_ASSIGN_XOR;
				break;
			case TOKEN_PUNCTUATOR_PIPE_EQUAL:
				op = AST_BINOP_ASSIGN_OR;
				break;
			default:
				op = AST_BINOP_NONE;
		}

		if (op != AST_BINOP_NONE)
		{
			Parser_ConsumeToken(self);
			AstExpression* rhs = Parser_ParseAssignmentExpression(self);
			if (!rhs)
				return lhs;

			lhs = NewWith(AstExpression, Binary,
			              (AstBinaryExpression){ op, lhs, rhs },
			              SourceLocation_Concat(&lhs->location, &rhs->location));
		}
	}

	return lhs;
}

AstExpression* Parser_ParseExpression(Parser* self)
{
	AstExpression* lhs = Parser_ParseAssignmentExpression(self);
	if (!lhs)
		return NULL;

	while (Parser_MatchToken(self, TOKEN_PUNCTUATOR_COMMA, NULL))
	{
		AstExpression* rhs = Parser_ParseAssignmentExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(AstExpression, Binary, (AstBinaryExpression){ AST_BINOP_COMMA, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

AstDeclaration* Parser_TryParseDeclaration(Parser* self)
{
	// TODO: static_assert-declaration

	AstDeclarationSpecifiers* declSpecs = Parser_ParseDeclarationSpecifiers(self);
	if (!declSpecs)
		return NULL;

	// TODO: init-declarator-list

	SourceLocation endLocation = { 0 };
	if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_SEMICOLON, &endLocation))
	{
		CompilerErrorList_Append(self->errors, CompilerError_Create("expected ';' at end of declaration", declSpecs->location));
		Release(declSpecs);
		return NULL;
	}
	return NewWith(AstDeclaration, Args, declSpecs,
	               SourceLocation_Concat(&declSpecs->location, &endLocation));
}

AstDeclarationSpecifiers* Parser_ParseDeclarationSpecifiers(Parser* self)
{
	SourceLocation locationStart = { 0 };
	SourceLocation locationEnd = { 0 };

	AstStorageClassSpecifierList* storageClassSpecifiers = New(AstStorageClassSpecifierList);
	AstTypeSpecifierList* typeSpecifiers = New(AstTypeSpecifierList);
	AstTypeQualifiers typeQualifiers = AST_TYPEQUALIFIERS_NONE;
	AstFunctionSpecifiers functionSpecifiers = AST_FUNCTIONSPECIFIERS_NONE;

	while (true)
	{
		AstStorageClassSpecifier* storageClassSpecifier = Parser_TryParseStorageClassSpecifier(self);
		if (storageClassSpecifier)
		{
			if (!locationStart.sourceFile)
				locationStart = locationEnd;

			locationEnd = storageClassSpecifier->location;

			AstStorageClassSpecifierList_Append(storageClassSpecifiers, storageClassSpecifier);
			continue;
		}

		AstTypeSpecifier* typeSpecifier = Parser_TryParseTypeSpecifier(self);
		if (typeSpecifier)
		{
			if (!locationStart.sourceFile)
				locationStart = locationEnd;

			locationEnd = typeSpecifier->location;

			AstTypeSpecifierList_Append(typeSpecifiers, typeSpecifier);
			continue;
		}

		const AstTypeQualifiers typeQualifier = Parser_TryParseTypeQualifier(self, &locationEnd);
		if (typeQualifier)
		{
			if (!locationStart.sourceFile)
				locationStart = locationEnd;

			typeQualifiers |= typeQualifier;
			continue;
		}

		const AstFunctionSpecifiers functionSpecifier = Parser_TryParseFunctionSpecifier(self, &locationEnd);
		if (functionSpecifier)
		{
			if (!locationStart.sourceFile)
				locationStart = locationEnd;

			functionSpecifiers |= functionSpecifier;
			continue;
		}

		// TODO: alignment specifiers

		break;
	}

	if (storageClassSpecifiers->size == 0 && typeSpecifiers->size == 0 && !typeQualifiers && !functionSpecifiers)
	{
		Release(storageClassSpecifiers);
		Release(typeSpecifiers);
		const Token* token = Parser_PeekToken(self);
		CompilerErrorList_Append(self->errors, CompilerError_Create("expected declaration specifier", token->location));
		return NULL;
	}

	return NewWith(AstDeclarationSpecifiers, Args, storageClassSpecifiers, typeSpecifiers, typeQualifiers, functionSpecifiers,
	               SourceLocation_Concat(&locationStart, &locationEnd));
}

AstStorageClassSpecifier* Parser_TryParseStorageClassSpecifier(Parser* self)
{
	const Token* token = Parser_PeekToken(self);
	if (token == NULL)
		return NULL;

	AstStorageClassSpecifier_Type type;
	switch (token->type)
	{
		case TOKEN_KEYWORD_AUTO:
			type = AST_STORAGECLASSSPECIFIER_AUTO;
			break;
		case TOKEN_KEYWORD_EXTERN:
			type = AST_STORAGECLASSSPECIFIER_EXTERN;
			break;
		case TOKEN_KEYWORD_REGISTER:
			type = AST_STORAGECLASSSPECIFIER_REGISTER;
			break;
		case TOKEN_KEYWORD_STATIC:
			type = AST_STORAGECLASSSPECIFIER_STATIC;
			break;
		case TOKEN_KEYWORD_TYPEDEF:
			type = AST_STORAGECLASSSPECIFIER_TYPEDEF;
			break;
		default:
			type = AST_STORAGECLASSSPECIFIER_NONE;
			break;
	}

	// TODO: thread-local storage specifiers

	if (type != AST_STORAGECLASSSPECIFIER_NONE)
	{
		Parser_ConsumeToken(self);
		return NewWith(AstStorageClassSpecifier, Args, type, token->location);
	}

	return NULL;
}

AstTypeSpecifier* Parser_TryParseTypeSpecifier(Parser* self)
{
	const Token* token = Parser_PeekToken(self);
	if (token == NULL)
		return NULL;

	AstTypeSpecifier_Type type;
	switch (token->type)
	{
		case TOKEN_KEYWORD_VOID:
			type = AST_TYPESPECIFIER_VOID;
			break;
		case TOKEN_KEYWORD_CHAR:
			type = AST_TYPESPECIFIER_CHAR;
			break;
		case TOKEN_KEYWORD_SHORT:
			type = AST_TYPESPECIFIER_SHORT;
			break;
		case TOKEN_KEYWORD_INT:
			type = AST_TYPESPECIFIER_INT;
			break;
		case TOKEN_KEYWORD_LONG:
			type = AST_TYPESPECIFIER_LONG;
			break;
		case TOKEN_KEYWORD_FLOAT:
			type = AST_TYPESPECIFIER_FLOAT;
			break;
		case TOKEN_KEYWORD_DOUBLE:
			type = AST_TYPESPECIFIER_DOUBLE;
			break;
		case TOKEN_KEYWORD_SIGNED:
			type = AST_TYPESPECIFIER_SIGNED;
			break;
		case TOKEN_KEYWORD_UNSIGNED:
			type = AST_TYPESPECIFIER_UNSIGNED;
			break;
		case TOKEN_KEYWORD_STRUCT:
			type = AST_TYPESPECIFIER_STRUCT;
			break;
		case TOKEN_KEYWORD_UNION:
			type = AST_TYPESPECIFIER_UNION;
			break;
		case TOKEN_KEYWORD_ENUM:
			type = AST_TYPESPECIFIER_ENUM;
			break;
		default:
			// TODO: check if identifier is a typedef-name
			type = AST_TYPESPECIFIER_NONE;
			break;
	}

	if (type == AST_TYPESPECIFIER_STRUCT ||
	    type == AST_TYPESPECIFIER_UNION ||
	    type == AST_TYPESPECIFIER_ENUM)
	{
		// TODO: struct-or-union-specifier or enum-specifier
		abort();
	}

	if (type != AST_TYPESPECIFIER_NONE)
	{
		Parser_ConsumeToken(self);
		return NewWith(AstTypeSpecifier, Args, type, token->location);
	}

	return NULL;
}

AstTypeSpecifierQualifierList* Parser_TryParseSpecifierQualifierList(Parser* self)
{
	SourceLocation locationStart = { 0 };
	SourceLocation locationEnd = { 0 };

	AstTypeSpecifierList* specifiers = New(AstTypeSpecifierList);
	AstTypeQualifiers qualifiers = AST_TYPEQUALIFIERS_NONE;

	while (true)
	{
		AstTypeSpecifier* specifier = Parser_TryParseTypeSpecifier(self);
		if (specifier)
		{
			locationEnd = specifier->location;

			if (!locationStart.sourceFile)
				locationStart = locationEnd;

			AstTypeSpecifierList_Append(specifiers, specifier);
			continue;
		}

		const AstTypeQualifiers qualifier = Parser_TryParseTypeQualifier(self, &locationEnd);
		if (qualifier)
		{
			if (!locationStart.sourceFile)
				locationStart = locationEnd;

			qualifiers |= qualifier;
			continue;
		}

		// TODO: alignment specifiers

		break;
	}

	if (specifiers->size == 0 && !qualifiers)
	{
		Release(specifiers);
		return NULL;
	}

	return NewWith(AstTypeSpecifierQualifierList, Args, specifiers, qualifiers, SourceLocation_Concat(&locationStart, &locationEnd));
}

AstTypeQualifiers Parser_TryParseTypeQualifier(Parser* self, SourceLocation* outLocation)
{
	const Token* token = Parser_PeekToken(self);
	if (token == NULL)
		return AST_TYPEQUALIFIERS_NONE;

	AstTypeQualifiers type;
	switch (token->type)
	{
		case TOKEN_KEYWORD_CONST:
			type = AST_TYPEQUALIFIERS_CONST;
			break;
		case TOKEN_KEYWORD_VOLATILE:
			type = AST_TYPEQUALIFIERS_VOLATILE;
			break;
		case TOKEN_KEYWORD_RESTRICT:
			type = AST_TYPEQUALIFIERS_RESTRICT;
			break;
		default:
			type = AST_TYPEQUALIFIERS_NONE;
			break;
	}

	if (type != AST_TYPEQUALIFIERS_NONE)
	{
		Parser_ConsumeToken(self);
		if (outLocation)
			*outLocation = token->location;
		return type;
	}

	return AST_TYPEQUALIFIERS_NONE;
}

AstFunctionSpecifiers Parser_TryParseFunctionSpecifier(Parser* self, SourceLocation* outLocation)
{
	const Token* token = Parser_PeekToken(self);
	if (token == NULL)
		return AST_FUNCTIONSPECIFIERS_NONE;

	AstFunctionSpecifiers type;
	switch (token->type)
	{
		case TOKEN_KEYWORD_INLINE:
			type = AST_FUNCTIONSPECIFIERS_INLINE;
			break;
		default:
			type = AST_FUNCTIONSPECIFIERS_NONE;
			break;
	}

	if (type != AST_FUNCTIONSPECIFIERS_NONE)
	{
		Parser_ConsumeToken(self);
		if (outLocation)
			*outLocation = token->location;
		return type;
	}

	return AST_FUNCTIONSPECIFIERS_NONE;
}

AstDeclarator* Parser_TryParseDeclarator(Parser* self)
{
	AstPointer* pointer = Parser_TryParsePointer(self);
	AstDirectDeclarator* directDeclarator = Parser_TryParseDirectDeclarator(self);
	if (!directDeclarator)
	{
		Release(pointer);
		return NULL;
	}

	const SourceLocation locationStart = pointer ? pointer->location : directDeclarator->location;
	const SourceLocation loc = SourceLocation_Concat(&locationStart, &directDeclarator->location);
	return NewWith(AstDeclarator, Args, pointer, directDeclarator, loc);
}

AstDirectDeclarator* Parser_TryParseDirectDeclarator(Parser* self)
{
	const Token* token = Parser_PeekToken(self);
	if (token == NULL)
		return NULL;

	if (token->type == TOKEN_IDENTIFIER)
	{
		Parser_ConsumeToken(self);
		return NewWith(AstDirectDeclarator, Identifier, token->data.literalIdentifier.value, token->location);
	}

	if (token->type == TOKEN_PUNCTUATOR_PARENOPEN)
	{
		Parser_ConsumeToken(self);

		AstDeclarator* declarator = Parser_TryParseDeclarator(self);
		if (!declarator)
			return NULL;

		if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENCLOSE, NULL))
		{
			CompilerErrorList_Append(self->errors, CompilerError_Create("expected ')' in parenthesized declarator", declarator->location));
			Release(declarator);
			return NULL;
		}

		return NewWith(AstDirectDeclarator, Parenthesized, declarator, SourceLocation_Concat(&token->location, &declarator->location));
	}

	// TODO: array declarators, function declarators, etc.
	abort();
}

AstPointer* Parser_TryParsePointer(Parser* self)
{
	SourceLocation locationStart = { 0 };
	const size_t savedTokenIndex = self->currentTokenIndex;
	if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_ASTERISK, &locationStart))
		return NULL;

	SourceLocation locationEnd = { 0 };
	const AstTypeQualifiers typeQualifiers = Parser_TryParseTypeQualifierList(self, &locationEnd);
	if (!typeQualifiers)
	{
		self->currentTokenIndex = savedTokenIndex;
		return NULL;
	}

	return NewWith(AstPointer, Args, typeQualifiers, SourceLocation_Concat(&locationStart, &locationEnd));
}

AstTypeQualifiers Parser_TryParseTypeQualifierList(Parser* self, SourceLocation* outLocation)
{
	SourceLocation locationStart = { 0 };
	SourceLocation locationEnd = { 0 };

	AstTypeQualifiers qualifiers = AST_TYPEQUALIFIERS_NONE;

	while (true)
	{
		const AstTypeQualifiers qualifier = Parser_TryParseTypeQualifier(self, &locationEnd);
		if (qualifier)
		{
			if (!locationStart.sourceFile)
				locationStart = locationEnd;

			qualifiers |= qualifier;
			continue;
		}

		break;
	}

	if (outLocation && qualifiers)
		*outLocation = SourceLocation_Concat(&locationStart, &locationEnd);

	return qualifiers;
}

AstTypeName* Parser_TryParseTypeName(Parser* self) // type-name
{
	// specifier-qualifier-list
	AstTypeSpecifierQualifierList* specifierQualifierList = Parser_TryParseSpecifierQualifierList(self);

	if (!specifierQualifierList)
		return NULL;

	// TODO: optional abstract-declarator

	return NewWith(AstTypeName, Args, specifierQualifierList, specifierQualifierList->location);
}
