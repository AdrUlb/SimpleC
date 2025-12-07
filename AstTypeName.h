#pragma once

#include "SourceFile.h"

#define AST_SPECIFIER_ENUM_VALUES \
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

#define AST_QUALIFIER_ENUM_VALUES \
	X(NONE) \
	X(CONST) \
	X(RESTRICT) \
	X(VOLATILE)

typedef enum AstTypeSpecifier_Type
{
#define X(name) AST_SPECIFIER_##name,
	AST_SPECIFIER_ENUM_VALUES
#undef X
} AstType_Specifier_Type;

typedef enum AstTypeQualifier_Type
{
#define X(name) AST_QUALIFIER_##name,
	AST_QUALIFIER_ENUM_VALUES
#undef X
} AstQualifier_Type;

static const char* AstTypeSpecifier_Type_ToString(const AstType_Specifier_Type type)
{
	switch (type)
	{
#define X(name) case AST_SPECIFIER_##name: return #name;
		AST_SPECIFIER_ENUM_VALUES
#undef X
		default:
			return "<unknown>";
	}
}

static const char* AstTypeQualifier_Type_ToString(const AstQualifier_Type type)
{
	switch (type)
	{
#define X(name) case AST_QUALIFIER_##name: return #name;
		AST_QUALIFIER_ENUM_VALUES
#undef X
		default:
			return "<unknown>";
	}
}

typedef struct
{
	AstType_Specifier_Type type;
} AstTypeSpecifier;

static AstTypeSpecifier* AstTypeSpecifier_Init_WithArgs(AstTypeSpecifier* self, const AstType_Specifier_Type type)
{
	self->type = type;
	return self;
}

static void AstTypeSpecifier_Fini(const AstTypeSpecifier* self)
{
	(void)self;
}

typedef struct
{
	AstQualifier_Type type;
} AstTypeQualifier;

static AstTypeQualifier* AstTypeQualifier_Init_WithArgs(AstTypeQualifier* self, const AstQualifier_Type type)
{
	self->type = type;
	return self;
}

static void AstTypeQualifier_Fini(const AstTypeQualifier* self)
{
	(void)self;
}

#define LIST_TYPE AstTypeSpecifierList
#define LIST_ELEMENT_TYPE AstTypeSpecifier*
#include "Util/ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE AstTypeQualifierList
#define LIST_ELEMENT_TYPE AstTypeQualifier*
#include "Util/ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

typedef struct
{
	SourceLocation location;
	AstTypeSpecifierList* specifiers;
	AstTypeQualifierList* qualifiers;
} AstTypeName;

static AstTypeName* AstTypeName_Init_WithArgs(AstTypeName* self, AstTypeSpecifierList* specifiers, AstTypeQualifierList* qualifiers, const SourceLocation location)
{
	self->location = location;
	self->specifiers = specifiers;
	self->qualifiers = qualifiers;
	return self;
}

static void AstTypeName_Fini(const AstTypeName* self)
{
	for (size_t i = 0; i < self->specifiers->size; i++)
		Release(self->specifiers->data[i]);
	for (size_t i = 0; i < self->qualifiers->size; i++)
		Release(self->qualifiers->data[i]);
	Release(self->specifiers);
	Release(self->qualifiers);
	(void)self;
}
