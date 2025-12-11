#pragma once
#include "SourceFile.h"

nullable_begin

typedef struct
{
	const char* message;
	SourceLocation location;
} CompilerError;

#define LIST_TYPE CompilerErrorList
#define LIST_ELEMENT_TYPE CompilerError
nullable_end
#include "Util/ListDef.h"
nullable_begin
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

static CompilerError CompilerError_Create(const char* message, const SourceLocation location)
{
	return (CompilerError) { message, location };
}

static const char* ErrorMsg_UnterminatedComment = "unterminated comment";
static const char* ErrorMsg_UnterminatedStringLiteral = "unterminated string literal";
static const char* ErrorMsg_UnexpectedEndOfFile = "unexpected end of file";
static const char* ErrorMsg_InvalidNumericLiteral = "invalid numeric literal";
static const char* ErrorMsg_InvalidAssignmentTarget = "invalid assignment target";
static const char* ErrorMsg_ExpectedColonInConditionalExpression = "expected ':' in conditional expression";
static const char* ErrorMsg_ExpectedClosingParenthesisInCastExpression = "expected ')' in cast expression";
static const char* ErrorMsg_ExpectedClosingParenthesisInSizeofTypeExpression = "expected ')' in sizeof(<type>) expression";
static const char* ErrorMsg_ExpectedClosingBracketInSubscriptExpression = "expected ']' in subscript expression";
static const char* ErrorMsg_ExpectedClosingParenthesisInParenthesizedExpression = "expected ')' in parenthesized expression";

nullable_end
