#pragma once

#include "CompilerError.h"
#include "AstExpression.h"
#include "Token.h"

nullable_begin

typedef struct
{
	TokenList* tokens;
	CompilerErrorList* errors;
	size_t currentTokenIndex;
} Parser;

static Parser Parser_Create(TokenList* tokens, CompilerErrorList* errorList)
{
	return (Parser) {
		.tokens = tokens,
		.errors = errorList,
	};
}

AstExpression*nullable Parser_ParseExpression(Parser* self);

nullable_end
