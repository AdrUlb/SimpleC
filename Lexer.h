#pragma once

#include "Token.h"
#include "Util/String.h"

typedef struct Lexer
{
	ConstCharSpan source;
	size_t position;
	int line;
	int column;
} Lexer;

void Lexer_Init(Lexer* self, ConstCharSpan source);
Token Lexer_GetNextToken(Lexer* self, bool includeWhitespace, bool includeComments);
const char* Lexer_GetTokenTypeName(Token_Type type);
