#pragma once
#include "SourceFile.h"

typedef struct
{
	const char* message;
	SourceLocation location;
} CompilerError;

#define LIST_TYPE CompilerErrorList
#define LIST_ELEMENT_TYPE CompilerError
#include "Util/ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

always_inline CompilerError CompilerError_Create(const char* message, const SourceLocation location)
{
	return (CompilerError) { message, location };
}

static const char* ErrorMsg_UnterminatedComment = "unterminated comment";
static const char* ErrorMsg_UnterminatedStringLiteral = "unterminated string literal";
static const char* ErrorMsg_UnexpectedEndOfFile = "unexpected end of file";
static const char* ErrorMsg_InvalidNumericLiteral = "invalid numeric literal";
