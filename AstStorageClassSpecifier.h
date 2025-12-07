#pragma once

#define AST_STORAGECLASSSPECIFIER_ENUM_VALUES \
	X(NONE) \
	X(AUTO) \
	X(EXTERN) \
	X(REGISTER) \
	X(STATIC) \
	X(TYPEDEF)

typedef enum AstStorageClassSpecifier_Type
{
#define X(name) AST_STORAGECLASSSPECIFIER_##name,
	AST_STORAGECLASSSPECIFIER_ENUM_VALUES
#undef X
} AstStorageClassSpecifier_Type;

static const char* AstStorageClassSpecifier_Type_ToString(const AstStorageClassSpecifier_Type type)
{
	switch (type)
	{
#define X(name) case AST_STORAGECLASSSPECIFIER_##name: return #name;
		AST_STORAGECLASSSPECIFIER_ENUM_VALUES
#undef X
		default:
		return "<unknown>";
	}
}

typedef struct
{
	SourceLocation location;
	AstStorageClassSpecifier_Type type;
} AstStorageClassSpecifier;

static AstStorageClassSpecifier* AstStorageClassSpecifier_Init_WithArgs(AstStorageClassSpecifier* self, const AstStorageClassSpecifier_Type type, const SourceLocation location)
{
	self->type = type;
	self->location = location;
	return self;
}

static void AstStorageClassSpecifier_Fini(const AstStorageClassSpecifier* self)
{
	(void)self;
}

#define LIST_TYPE AstStorageClassSpecifierList
#define LIST_ELEMENT_TYPE AstStorageClassSpecifier*
#include "Util/ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE
