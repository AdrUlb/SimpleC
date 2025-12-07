#include "Parser.h"

#include <math.h>

#include "Util/Managed.h"

static AstExpression* Parser_ParseCastExpression(Parser* self);
static AstExpression* Parser_ParseAssignmentExpression(Parser* self);

static Token* Parser_PeekToken(const Parser* self)
{
	if (self->currentTokenIndex >= self->tokens->size)
		return NULL;

	return &self->tokens->data[self->currentTokenIndex];
}

static Token* Parser_ConsumeToken(Parser* self)
{
	if (self->currentTokenIndex >= self->tokens->size)
		return NULL;

	return &self->tokens->data[self->currentTokenIndex++];
}

static bool Parser_MatchToken(Parser* self, const Token_Type type, SourceLocation* outLocation)
{
	const Token* token = Parser_PeekToken(self);
	if (token == NULL || token->type != type)
		return false;

	Parser_ConsumeToken(self);
	if (outLocation != NULL)
		*outLocation = token->location;
	return true;
}

static bool Parser_ParseSpecifierQualifierList(Parser* self, AstSpecifierList** outSpecifiers, AstQualifierList** outQualifiers, SourceLocation* outLocation)
{
	// specifier-qualifier-list
	AstSpecifierList* specifiers = New(AstSpecifierList);
	AstQualifierList* qualifiers = New(AstQualifierList);

	*outSpecifiers = specifiers;
	*outQualifiers = qualifiers;

	SourceLocation locationStart = { 0 };
	SourceLocation locationEnd = { 0 };

	while (true)
	{
		const Token* token = Parser_PeekToken(self);
		if (token == NULL)
			return NULL;

		AstType_Specifier_Type specifierType;
		switch (token->type)
		{
			case TOKEN_KEYWORD_VOID:
				specifierType = AST_SPECIFIER_VOID;
				break;
			case TOKEN_KEYWORD_CHAR:
				specifierType = AST_SPECIFIER_CHAR;
				break;
			case TOKEN_KEYWORD_SHORT:
				specifierType = AST_SPECIFIER_SHORT;
				break;
			case TOKEN_KEYWORD_INT:
				specifierType = AST_SPECIFIER_INT;
				break;
			case TOKEN_KEYWORD_LONG:
				specifierType = AST_SPECIFIER_LONG;
				break;
			case TOKEN_KEYWORD_FLOAT:
				specifierType = AST_SPECIFIER_FLOAT;
				break;
			case TOKEN_KEYWORD_DOUBLE:
				specifierType = AST_SPECIFIER_DOUBLE;
				break;
			case TOKEN_KEYWORD_SIGNED:
				specifierType = AST_SPECIFIER_SIGNED;
				break;
			case TOKEN_KEYWORD_UNSIGNED:
				specifierType = AST_SPECIFIER_UNSIGNED;
				break;
			case TOKEN_KEYWORD_STRUCT:
				specifierType = AST_SPECIFIER_STRUCT;
				break;
			case TOKEN_KEYWORD_UNION:
				specifierType = AST_SPECIFIER_UNION;
				break;
			case TOKEN_KEYWORD_ENUM:
				specifierType = AST_SPECIFIER_ENUM;
				break;
			default:
				// TODO: check if identifier is a typedef-name
				specifierType = AST_SPECIFIER_NONE;
				break;
		}

		if (specifierType == AST_SPECIFIER_STRUCT ||
		    specifierType == AST_SPECIFIER_UNION ||
		    specifierType == AST_SPECIFIER_ENUM)
		{
			// TODO: struct-or-union-specifier or enum-specifier
			abort();
		}

		if (specifierType != AST_SPECIFIER_NONE)
		{
			if (!locationStart.sourceFile)
				locationStart = token->location;

			locationEnd = token->location;

			Parser_ConsumeToken(self);
			AstSpecifierList_Append(specifiers, NewWith(AstSpecifier, Args, specifierType));
		}

		AstQualifier_Type qualifierType;
		switch (token->type)
		{
			case TOKEN_KEYWORD_CONST:
				qualifierType = AST_QUALIFIER_CONST;
				break;
			case TOKEN_KEYWORD_VOLATILE:
				qualifierType = AST_QUALIFIER_VOLATILE;
				break;
			case TOKEN_KEYWORD_RESTRICT:
				qualifierType = AST_QUALIFIER_RESTRICT;
				break;
			default:
				qualifierType = AST_QUALIFIER_NONE;
				break;
		}

		if (qualifierType != AST_QUALIFIER_NONE)
		{
			if (!locationStart.sourceFile)
				locationStart = token->location;

			locationEnd = token->location;

			Parser_ConsumeToken(self);
			AstQualifierList_Append(qualifiers, NewWith(AstQualifier, Args, qualifierType));
		}

		// TODO: alignment specifiers

		if (specifierType == AST_SPECIFIER_NONE && qualifierType == AST_QUALIFIER_NONE)
			break;
	}

	if (specifiers->size == 0 && qualifiers->size == 0)
	{
		Release(specifiers);
		Release(qualifiers);
		return false;
	}

	*outLocation = SourceLocation_Concat(&locationStart, &locationEnd);
	return true;
}

static AstTypeName* Parser_TryParseTypeName(Parser* self) // type-name
{
	// specifier-qualifier-list
	AstSpecifierList* specifiers;
	AstQualifierList* qualifiers;
	SourceLocation specifierQualifierLocation = { 0 };

	if (!Parser_ParseSpecifierQualifierList(self, &specifiers, &qualifiers, &specifierQualifierLocation))
		return NULL;

	// TODO: optional abstract-declarator

	return NewWith(AstTypeName, Args, specifiers, qualifiers, specifierQualifierLocation);
}

static AstExpression* Parser_ParsePrimaryExpression(Parser* self)
{
	const Token* token = Parser_PeekToken(self);
	if (token == NULL)
		return NULL;

	if (token->type == TOKEN_PUNCTUATOR_PARENOPEN)
	{
		// Parenthesized expression
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

	if (token->type == TOKEN_LITERAL_INTEGER ||
	    token->type == TOKEN_LITERAL_FLOAT ||
	    token->type == TOKEN_LITERAL_CHAR ||
	    token->type == TOKEN_LITERAL_STRING ||
	    token->type == TOKEN_IDENTIFIER)
	{
		Parser_ConsumeToken(self);

		return NewWith(AstExpression, Primary,
		               (AstPrimaryExpression){ *token },
		               token->location);
	}

	return NULL;
}

static AstExpression* Parser_ParsePostfixExpression(Parser* self)
{
	const size_t savedTokenIndex = self->currentTokenIndex;

	AstExpression* expression = NULL;
	if (true)
	{
		if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENOPEN, NULL))
			goto primary;

		const AstTypeName* type = Parser_TryParseTypeName(self);
		if (!type)
			goto primary;

		if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENCLOSE, NULL) || !Parser_MatchToken(self, TOKEN_PUNCTUATOR_BRACEOPEN, NULL))
		{
			Release(type);
			goto primary;
		}

		// TOOD: initializer list
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

static AstExpression* Parser_ParseUnaryExpression(Parser* self)
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

static AstExpression* Parser_ParseMultiplicativeExpression(Parser* self)
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

static AstExpression* Parser_ParseAdditiveExpression(Parser* self)
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

static AstExpression* Parser_ParseShiftExpression(Parser* self)
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

static AstExpression* Parser_ParseRelationalExpression(Parser* self)
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

static AstExpression* Parser_ParseEqualityExpression(Parser* self)
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

static AstExpression* Parser_ParseBitwiseAndExpression(Parser* self)
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

static AstExpression* Parser_ParseBitwiseXorExpression(Parser* self)
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

static AstExpression* Parser_ParseBitwiseOrExpression(Parser* self)
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

static AstExpression* Parser_ParseLogicalAndExpression(Parser* self)
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

static AstExpression* Parser_ParseLogicalOrExpression(Parser* self)
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

static AstExpression* Parser_ParseConditionalExpression(Parser* self)
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
			/* TODO: FIXME
			if (!Expression_IsLValue(lhs))
				CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_InvalidAssignmentTarget, lhs->location));
			*/

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
