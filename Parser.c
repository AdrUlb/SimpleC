#include "Parser.h"

#include <math.h>

#include "Util/Managed.h"

static Expression* Parser_ParseCastExpression(Parser* self);
static Expression* Parser_ParseAssignmentExpression(Parser* self);

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

static Type* Parser_ParseType(const Parser* self)
{
	// TODO
	return NULL;
}

static Expression* Parser_ParsePrimaryExpression(Parser* self)
{
	const Token* token = Parser_PeekToken(self);
	if (token == NULL)
		return NULL;

	if (token->type == TOKEN_PUNCTUATOR_PARENOPEN)
	{
		// Parenthesized expression
		Parser_ConsumeToken(self);

		Expression* expr = Parser_ParseExpression(self);
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

		return NewWith(Expression, Primary,
		               (PrimaryExpression){ *token },
		               token->location);
	}

	return NULL;
}

static Expression* Parser_ParsePostfixExpression(Parser* self)
{
	const size_t savedTokenIndex = self->currentTokenIndex;

	Expression* expression = NULL;
	if (true)
	{
		if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENOPEN, NULL))
			goto primary;

		const Type* type = Parser_ParseType(self);
		if (!type)
			goto primary;

		if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENCLOSE, NULL))
			goto primary;

		if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_BRACEOPEN, NULL))
			goto primary;

		// TOOD: initializer list
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
			Expression* indexExpr = Parser_ParseExpression(self);
			if (!indexExpr)
				return NULL;

			if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_BRACKETCLOSE, NULL))
			{
				CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_ExpectedClosingBracketInSubscriptExpression, indexExpr->location));
				return NULL;
			}

			expression = NewWith(Expression, Binary,
			                     (BinaryExpression){ BINOP_SUBSCRIPT, expression, indexExpr },
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

			expression = NewWith(Expression, MemberAccess,
			                     (MemberAccessExpression){ expression, indentifier->data.literalIdentifier.value,
			                     token->type == TOKEN_PUNCTUATOR_MINUS_GREATER },
			                     SourceLocation_Concat(&expression->location, &indentifier->location));
			continue;
		}

		if (token->type == TOKEN_PUNCTUATOR_PLUS_PLUS || token->type == TOKEN_PUNCTUATOR_MINUS_MINUS)
		{
			// Postfix increment/decrement
			Parser_ConsumeToken(self);

			const UnaryOperation op = (token->type == TOKEN_PUNCTUATOR_PLUS_PLUS) ? UNOP_POST_INCREMENT : UNOP_POST_DECREMENT;

			expression = NewWith(Expression, Unary,
			                     (UnaryExpression){ op, expression },
			                     SourceLocation_Concat(&expression->location, &token->location));
			continue;
		}

		if (token->type == TOKEN_PUNCTUATOR_PARENOPEN)
		{
			// Function call
			Parser_ConsumeToken(self);

			using ExpressionList* args = New(ExpressionList);
			while (true)
			{
				if (Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENCLOSE, NULL))
					break;

				Expression* argExpr = Parser_ParseAssignmentExpression(self);
				if (!argExpr)
					return NULL;

				ExpressionList_Append(args, argExpr);

				if (Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENCLOSE, NULL))
					break;

				if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_COMMA, NULL))
				{
					CompilerErrorList_Append(self->errors, CompilerError_Create("expected ',' or ')' in function call argument list", argExpr->location));
					return NULL;
				}
			}

			expression = NewWith(Expression, Call,
			                     (CallExpression){ expression, Retain(args) },
			                     SourceLocation_Concat(&expression->location, &token->location));
			continue;
		}
		break;
	}

	return expression;
}

static Expression* Parser_ParseUnaryExpression(Parser* self)
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
			Type* type = Parser_ParseType(self);
			if (!type)
				return NULL;

			if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENCLOSE, NULL))
			{
				CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_ExpectedClosingParenthesisInSizeofTypeExpression, type->location));
				return NULL;
			}

			return NewWith(Expression, SizeofType,
			               (SizeofTypeExpression){ type },
			               SourceLocation_Concat(&token->location, &type->location));
		}

		// sizeof <expression>
		Expression* expr = Parser_ParseUnaryExpression(self);
		if (!expr)
			return NULL;

		return NewWith(Expression, Unary,
		               (UnaryExpression){ UNOP_SIZEOF, expr },
		               SourceLocation_Concat(&token->location, &expr->location));
	}

	// Prefix expressions
	UnaryOperation op;
	switch (token->type)
	{
		case TOKEN_PUNCTUATOR_AMPERSAND:
			op = UNOP_ADDRESS_OF;
			break;
		case TOKEN_PUNCTUATOR_ASTERISK:
			op = UNOP_DEREFERENCE;
			break;
		case TOKEN_PUNCTUATOR_PLUS:
			op = UNOP_PLUS;
			break;
		case TOKEN_PUNCTUATOR_MINUS:
			op = UNOP_MINUS;
			break;
		case TOKEN_PUNCTUATOR_TILDE:
			op = UNOP_BITWISE_NOT;
			break;
		case TOKEN_PUNCTUATOR_EXCLAMATION:
			op = UNOP_LOGICAL_NOT;
			break;
		case TOKEN_PUNCTUATOR_PLUS_PLUS:
			op = UNOP_PRE_INCREMENT;
			break;
		case TOKEN_PUNCTUATOR_MINUS_MINUS:
			op = UNOP_PRE_DECREMENT;
			break;
		default:
			op = UNOP_NONE;
	}

	if (op == UNOP_PRE_INCREMENT || op == UNOP_PRE_DECREMENT)
	{
		Parser_ConsumeToken(self);
		Expression* expr = Parser_ParseUnaryExpression(self);
		if (!expr)
			return NULL;

		return NewWith(Expression, Unary,
		               (UnaryExpression){ op, expr },
		               SourceLocation_Concat(&token->location, &expr->location));
	}

	if (op != UNOP_NONE)
	{
		Parser_ConsumeToken(self);
		Expression* expr = Parser_ParseCastExpression(self);
		if (!expr)
			return NULL;

		return NewWith(Expression, Unary,
		               (UnaryExpression){ op, expr },
		               SourceLocation_Concat(&token->location, &expr->location));
	}

	return Parser_ParsePostfixExpression(self);
}

Expression* Parser_ParseCastExpression(Parser* self)
{
	const size_t savedTokenIndex = self->currentTokenIndex;

	SourceLocation startLocation;
	if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENOPEN, &startLocation))
		return Parser_ParseUnaryExpression(self);

	Type* type = Parser_ParseType(self);
	if (!type)
	{
		// Not a cast expression, rewind and parse as unary expression
		self->currentTokenIndex = savedTokenIndex;
		return Parser_ParseUnaryExpression(self);
	}

	if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_PARENCLOSE, NULL))
	{
		CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_ExpectedClosingParenthesisInCastExpression, type->location));
		return NULL;
	}

	Expression* expr = Parser_ParseCastExpression(self);
	return NewWith(Expression, Cast, (CastExpression){ type, expr }, SourceLocation_Concat(&startLocation, &expr->location));
}

static Expression* Parser_ParseMultiplicativeExpression(Parser* self)
{
	Expression* lhs = Parser_ParseCastExpression(self);
	if (!lhs)
		return NULL;

	while (true)
	{
		const Token* token = Parser_PeekToken(self);
		if (token == NULL)
			break;

		BinaryOperation op;
		switch (token->type)
		{
			case TOKEN_PUNCTUATOR_ASTERISK:
				op = BINOP_MULTIPLY;
				break;
			case TOKEN_PUNCTUATOR_SLASH:
				op = BINOP_DIVIDE;
				break;
			case TOKEN_PUNCTUATOR_PERCENT:
				op = BINOP_MODULO;
				break;
			default:
				op = BINOP_NONE;
		}

		if (op == BINOP_NONE)
			break;

		Parser_ConsumeToken(self);
		Expression* rhs = Parser_ParseCastExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(Expression, Binary, (BinaryExpression){ op, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

static Expression* Parser_ParseAdditiveExpression(Parser* self)
{
	Expression* lhs = Parser_ParseMultiplicativeExpression(self);
	if (!lhs)
		return NULL;

	while (true)
	{
		const Token* token = Parser_PeekToken(self);
		if (token == NULL)
			break;

		BinaryOperation op;
		switch (token->type)
		{
			case TOKEN_PUNCTUATOR_PLUS:
				op = BINOP_ADD;
				break;
			case TOKEN_PUNCTUATOR_MINUS:
				op = BINOP_SUBTRACT;
				break;
			default:
				op = BINOP_NONE;
		}

		if (op == BINOP_NONE)
			break;

		Parser_ConsumeToken(self);
		Expression* rhs = Parser_ParseMultiplicativeExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(Expression, Binary, (BinaryExpression){ op, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

static Expression* Parser_ParseShiftExpression(Parser* self)
{
	Expression* lhs = Parser_ParseAdditiveExpression(self);
	if (!lhs)
		return NULL;

	while (true)
	{
		const Token* token = Parser_PeekToken(self);
		if (token == NULL)
			break;

		BinaryOperation op;
		switch (token->type)
		{
			case TOKEN_PUNCTUATOR_LESS_LESS:
				op = BINOP_SHIFT_LEFT;
				break;
			case TOKEN_PUNCTUATOR_GREATER_GREATER:
				op = BINOP_SHIFT_RIGHT;
				break;
			default:
				op = BINOP_NONE;
		}

		if (op == BINOP_NONE)
			break;

		Parser_ConsumeToken(self);
		Expression* rhs = Parser_ParseAdditiveExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(Expression, Binary, (BinaryExpression){ op, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

static Expression* Parser_ParseRelationalExpression(Parser* self)
{
	Expression* lhs = Parser_ParseShiftExpression(self);
	if (!lhs)
		return NULL;

	while (true)
	{
		const Token* token = Parser_PeekToken(self);
		if (token == NULL)
			break;

		BinaryOperation op;
		switch (token->type)
		{
			case TOKEN_PUNCTUATOR_LESS:
				op = BINOP_TEST_LESS;
				break;
			case TOKEN_PUNCTUATOR_GREATER:
				op = BINOP_TEST_GREATER;
				break;
			case TOKEN_PUNCTUATOR_LESS_EQUAL:
				op = BINOP_TEST_LESS_EQUAL;
				break;
			case TOKEN_PUNCTUATOR_GREATER_EQUAL:
				op = BINOP_TEST_GREATER_EQUAL;
				break;
			default:
				op = BINOP_NONE;
		}

		if (op == BINOP_NONE)
			break;

		Parser_ConsumeToken(self);
		Expression* rhs = Parser_ParseShiftExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(Expression, Binary, (BinaryExpression){ op, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

static Expression* Parser_ParseEqualityExpression(Parser* self)
{
	Expression* lhs = Parser_ParseRelationalExpression(self);
	if (!lhs)
		return NULL;

	while (true)
	{
		const Token* token = Parser_PeekToken(self);
		if (token == NULL)
			break;

		BinaryOperation op;
		switch (token->type)
		{
			case TOKEN_PUNCTUATOR_EQUAL_EQUAL:
				op = BINOP_TEST_EQUAL;
				break;
			case TOKEN_PUNCTUATOR_EXCLAMATION_EQUAL:
				op = BINOP_TEST_NOT_EQUAL;
				break;
			default:
				op = BINOP_NONE;
		}

		if (op == BINOP_NONE)
			break;

		Parser_ConsumeToken(self);
		Expression* rhs = Parser_ParseRelationalExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(Expression, Binary, (BinaryExpression){ op, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

static Expression* Parser_ParseBitwiseAndExpression(Parser* self)
{
	Expression* lhs = Parser_ParseEqualityExpression(self);
	if (!lhs)
		return NULL;

	while (Parser_MatchToken(self, TOKEN_PUNCTUATOR_AMPERSAND, NULL))
	{
		Expression* rhs = Parser_ParseEqualityExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(Expression, Binary, (BinaryExpression){ BINOP_BITWISE_AND, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

static Expression* Parser_ParseBitwiseXorExpression(Parser* self)
{
	Expression* lhs = Parser_ParseBitwiseAndExpression(self);
	if (!lhs)
		return NULL;

	while (Parser_MatchToken(self, TOKEN_PUNCTUATOR_CARET, NULL))
	{
		Expression* rhs = Parser_ParseBitwiseAndExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(Expression, Binary, (BinaryExpression){ BINOP_BITWISE_XOR, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

static Expression* Parser_ParseBitwiseOrExpression(Parser* self)
{
	Expression* lhs = Parser_ParseBitwiseXorExpression(self);
	if (!lhs)
		return NULL;

	while (Parser_MatchToken(self, TOKEN_PUNCTUATOR_PIPE, NULL))
	{
		Expression* rhs = Parser_ParseBitwiseXorExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(Expression, Binary, (BinaryExpression){ BINOP_BITWISE_OR, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

static Expression* Parser_ParseLogicalAndExpression(Parser* self)
{
	Expression* lhs = Parser_ParseBitwiseOrExpression(self);
	if (!lhs)
		return NULL;

	while (Parser_MatchToken(self, TOKEN_PUNCTUATOR_AMPERSAND_AMPERSAND, NULL))
	{
		Expression* rhs = Parser_ParseBitwiseOrExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(Expression, Binary, (BinaryExpression){ BINOP_LOGICAL_AND, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

static Expression* Parser_ParseLogicalOrExpression(Parser* self)
{
	Expression* lhs = Parser_ParseLogicalAndExpression(self);
	if (!lhs)
		return NULL;

	while (Parser_MatchToken(self, TOKEN_PUNCTUATOR_PIPE_PIPE, NULL))
	{
		Expression* rhs = Parser_ParseLogicalAndExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(Expression, Binary, (BinaryExpression){ BINOP_LOGICAL_OR, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}

static Expression* Parser_ParseConditionalExpression(Parser* self)
{
	Expression* lhs = Parser_ParseLogicalOrExpression(self);
	if (!lhs)
		return NULL;

	while (Parser_MatchToken(self, TOKEN_PUNCTUATOR_QUESTION, NULL))
	{
		Expression* ifTrue = Parser_ParseExpression(self);
		if (!ifTrue)
			break;

		if (!Parser_MatchToken(self, TOKEN_PUNCTUATOR_COLON, NULL))
		{
			CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_ExpectedColonInConditionalExpression, lhs->location));
			break;
		}

		Expression* ifFalse = Parser_ParseConditionalExpression(self);
		if (!ifFalse)
			break;

		lhs = NewWith(Expression, Ternary,
		              (TernaryExpression){ TERNOP_CONDITIONAL, lhs, ifTrue, ifFalse },
		              SourceLocation_Concat(&lhs->location, &ifFalse->location));
	}

	return lhs;
}

Expression* Parser_ParseAssignmentExpression(Parser* self)
{
	Expression* lhs = Parser_ParseConditionalExpression(self);
	if (!lhs)
		return NULL;

	const Token* token = Parser_PeekToken(self);
	if (token != NULL)
	{
		BinaryOperation op;
		switch (token->type)
		{
			case TOKEN_PUNCTUATOR_EQUAL:
				op = BINOP_ASSIGN;
				break;
			case TOKEN_PUNCTUATOR_ASTERISK_EQUAL:
				op = BINOP_ASSIGN_MULTIPLY;
				break;
			case TOKEN_PUNCTUATOR_SLASH_EQUAL:
				op = BINOP_ASSIGN_DIVIDE;
				break;
			case TOKEN_PUNCTUATOR_PERCENT_EQUAL:
				op = BINOP_ASSIGN_MODULO;
				break;
			case TOKEN_PUNCTUATOR_PLUS_EQUAL:
				op = BINOP_ASSIGN_ADD;
				break;
			case TOKEN_PUNCTUATOR_MINUS_EQUAL:
				op = BINOP_ASSIGN_SUBTRACT;
				break;
			case TOKEN_PUNCTUATOR_LESS_LESS_EQUAL:
				op = BINOP_ASSIGN_LEFT_SHIFT;
				break;
			case TOKEN_PUNCTUATOR_GREATER_GREATER_EQUAL:
				op = BINOP_ASSIGN_RIGHT_SHIFT;
				break;
			case TOKEN_PUNCTUATOR_AMPERSAND_EQUAL:
				op = BINOP_ASSIGN_AND;
				break;
			case TOKEN_PUNCTUATOR_CARET_EQUAL:
				op = BINOP_ASSIGN_XOR;
				break;
			case TOKEN_PUNCTUATOR_PIPE_EQUAL:
				op = BINOP_ASSIGN_OR;
				break;
			default:
				op = BINOP_NONE;
		}

		if (op != BINOP_NONE)
		{
			/* TODO: FIXME
			if (!Expression_IsLValue(lhs))
				CompilerErrorList_Append(self->errors, CompilerError_Create(ErrorMsg_InvalidAssignmentTarget, lhs->location));
			*/

			Parser_ConsumeToken(self);
			Expression* rhs = Parser_ParseAssignmentExpression(self);
			if (!rhs)
				return lhs;

			lhs = NewWith(Expression, Binary,
			              (BinaryExpression){ op, lhs, rhs },
			              SourceLocation_Concat(&lhs->location, &rhs->location));
		}
	}

	return lhs;
}

Expression* Parser_ParseExpression(Parser* self)
{
	Expression* lhs = Parser_ParseAssignmentExpression(self);
	if (!lhs)
		return NULL;

	while (Parser_MatchToken(self, TOKEN_PUNCTUATOR_COMMA, NULL))
	{
		Expression* rhs = Parser_ParseAssignmentExpression(self);
		if (!rhs)
			return lhs;

		lhs = NewWith(Expression, Binary, (BinaryExpression){ BINOP_COMMA, lhs, rhs }, SourceLocation_Concat(&lhs->location, &rhs->location));
	}

	return lhs;
}
