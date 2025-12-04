#pragma once

#include "CompilerError.h"
#include "SourceFile.h"
#include "Token.h"

typedef struct Lexer
{
	const SourceFile* source;
	size_t position;
	int line;
	int column;
	CompilerErrorList* errorList;
} Lexer;

Lexer* Lexer_Init(Lexer* self, const SourceFile* source, CompilerErrorList* errorList);
void Lexer_Fini(const Lexer* self);
Token Lexer_GetNextToken(Lexer* self, bool includeWhitespace, bool includeComments);
const char* Lexer_GetTokenTypeName(Token_Type type);
