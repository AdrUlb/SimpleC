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
	CompilerErrorList* errors;
} Lexer;

Lexer Lexer_Create(const SourceFile* source, CompilerErrorList* errorList);
Token Lexer_GetNextToken(Lexer* self, bool includeWhitespace, bool includeComments);
