#pragma once

#include "CompilerError.h"
#include "Expression.h"
#include "Token.h"
#include "Util/Macros.h"

typedef struct
{
	TokenList* tokens;
	CompilerErrorList* errors;
	size_t currentTokenIndex;
} Parser;

always_inline Parser Parser_Create(TokenList* tokens, CompilerErrorList* errorList)
{
	return (Parser) {
		.tokens = tokens,
		.errors = errorList,
	};
}

Expression* Parser_ParseExpression(Parser* self);
