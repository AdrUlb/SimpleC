#pragma once

#define AST_TYPEQUALIFIER_ENUM_VALUES \
X(NONE) \
X(CONST) \
X(RESTRICT) \
X(VOLATILE)

typedef enum AstTypeQualifier_Type
{
#define X(name) AST_TYPEQUALIFIER_##name,
	AST_TYPEQUALIFIER_ENUM_VALUES
#undef X
} AstTypeQualifier_Type;

static const char* AstTypeQualifier_Type_ToString(const AstTypeQualifier_Type type)
{
	switch (type)
	{
#define X(name) case AST_TYPEQUALIFIER_##name: return #name;
		AST_TYPEQUALIFIER_ENUM_VALUES
#undef X
		default:
			return "<unknown>";
	}
}

typedef struct
{
	SourceLocation location;
	AstTypeQualifier_Type type;
} AstTypeQualifier;

static AstTypeQualifier* AstTypeQualifier_Init_WithArgs(AstTypeQualifier* self, const AstTypeQualifier_Type type, const SourceLocation location)
{
	self->type = type;
	self->location = location;
	return self;
}

static void AstTypeQualifier_Fini(const AstTypeQualifier* self)
{
	(void)self;
}

#define LIST_TYPE AstTypeQualifierList
#define LIST_ELEMENT_TYPE AstTypeQualifier*
#include "Util/ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE
