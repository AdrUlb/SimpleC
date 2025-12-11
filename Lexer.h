#pragma once

#include "CompilerError.h"
#include "SourceFile.h"
#include "Token.h"

nullable_begin

typedef struct
{
	const SourceFile* source;
	size_t position;
	size_t line;
	size_t column;
	CompilerErrorList* errors;
} Lexer;

Lexer Lexer_Create(const SourceFile* source, CompilerErrorList* errorList);
Token Lexer_GetNextToken(Lexer* self, bool includeWhitespace, bool includeComments);

nullable_end
