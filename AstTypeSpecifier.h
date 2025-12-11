#pragma once

nullable_begin

#define AST_TYPESPECIFIER_ENUM_VALUES \
X(NONE) \
X(VOID) \
X(CHAR) \
X(SHORT) \
X(INT) \
X(LONG) \
X(FLOAT) \
X(DOUBLE) \
X(SIGNED) \
X(UNSIGNED) \
X(STRUCT) \
X(UNION) \
X(ENUM) \
X(TYPEDEF_NAME)

typedef enum AstTypeSpecifier_Type
{
#define X(name) AST_TYPESPECIFIER_##name,
	AST_TYPESPECIFIER_ENUM_VALUES
#undef X
} AstTypeSpecifier_Type;

static const char* AstTypeSpecifier_Type_ToString(const AstTypeSpecifier_Type type)
{
	switch (type)
	{
#define X(name) case AST_TYPESPECIFIER_##name: return #name;
		AST_TYPESPECIFIER_ENUM_VALUES
#undef X
		default:
			return "<unknown>";
	}
}

typedef struct
{
	SourceLocation location;
	AstTypeSpecifier_Type type;
} AstTypeSpecifier;

static AstTypeSpecifier* AstTypeSpecifier_Init_WithArgs(AstTypeSpecifier* self, const AstTypeSpecifier_Type type, const SourceLocation location)
{
	self->type = type;
	self->location = location;
	return self;
}

static void AstTypeSpecifier_Fini(const AstTypeSpecifier* self)
{
	(void)self;
}

nullable_end

#define LIST_TYPE AstTypeSpecifierList
#define LIST_ELEMENT_TYPE AstTypeSpecifier*
#include "Util/ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE
