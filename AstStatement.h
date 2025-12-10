#pragma once

#include "SourceFile.h"

#define AST_STMT_ENUM_VALUES \
	/* labeled statement */ \
	X(LABEL) \
	X(CASE) \
	X(DEFAULT) \
	/* compound-statement */ \
	X(COMPOUND) \
	/* expression-statement */ \
	X(EXPRESSION) \
	/* selection-statement */ \
	X(IF) \
	X(SWITCH) \
	/* iteration-statement */ \
	X(WHILE) \
	X(DOWHILE) \
	X(FOR) \
	/* jump-statement */ \
	X(GOTO_STMT) \
	X(CONTINUE_STMT) \
	X(BREAK_STMT) \
	X(RETURN_STMT)

typedef enum
{
#define X(name) AST_STMT_##name,
	AST_STMT_ENUM_VALUES
#undef X
} AstStatement_Type;

static const char* AstStatement_Type_ToString(const AstStatement_Type type)
{
	switch (type)
	{
#define X(name) case AST_STMT_##name: return #name;
		AST_STMT_ENUM_VALUES
#undef X
		default:
			return "<unknown>";
	}
}

typedef struct
{
	SourceLocation location;
	AstStatement_Type type;
} AstStatement;

#define LIST_TYPE AstStatementList
#define LIST_ELEMENT_TYPE AstStatement*
#include "Util/ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE
