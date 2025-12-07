#pragma once

#define AST_FUNCTIONSPECIFIER_ENUM_VALUES \
X(NONE) \
X(INLINE) \

typedef enum AstFunctionSpecifier_Type
{
#define X(name) AST_FUNCTIONSPECIFIER_##name,
	AST_FUNCTIONSPECIFIER_ENUM_VALUES
#undef X
} AstFunctionSpecifier_Type;

static const char* AstFunctionSpecifier_Type_ToString(const AstFunctionSpecifier_Type type)
{
	switch (type)
	{
#define X(name) case AST_FUNCTIONSPECIFIER_##name: return #name;
		AST_FUNCTIONSPECIFIER_ENUM_VALUES
#undef X
		default:
		return "<unknown>";
	}
}

typedef struct
{
	SourceLocation location;
	AstFunctionSpecifier_Type type;
} AstFunctionSpecifier;

static AstFunctionSpecifier* AstFunctionSpecifier_Init_WithArgs(AstFunctionSpecifier* self, const AstFunctionSpecifier_Type type, const SourceLocation location)
{
	self->type = type;
	self->location = location;
	return self;
}

static void AstFunctionSpecifier_Fini(const AstFunctionSpecifier* self)
{
	(void)self;
}

#define LIST_TYPE AstFunctionSpecifierList
#define LIST_ELEMENT_TYPE AstFunctionSpecifier*
#include "Util/ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE
