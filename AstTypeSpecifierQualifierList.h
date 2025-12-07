#pragma once

#include "AstTypeQualifier.h"
#include "AstTypeSpecifier.h"
#include "SourceFile.h"

typedef struct
{
	SourceLocation location;
	AstTypeSpecifierList* specifiers;
	AstTypeQualifierList* qualifiers;
} AstTypeSpecifierQualifierList;

static AstTypeSpecifierQualifierList* AstTypeSpecifierQualifierList_Init_WithArgs(AstTypeSpecifierQualifierList* self,
                                                                                  AstTypeSpecifierList* specifiers,
                                                                                  AstTypeQualifierList* qualifiers,
                                                                                  const SourceLocation location)
{
	self->specifiers = specifiers;
	self->qualifiers = qualifiers;
	self->location = location;
	return self;
}

static void AstTypeSpecifierQualifierList_Fini(const AstTypeSpecifierQualifierList* self)
{
	for (size_t i = 0; i < self->specifiers->size; i++)
		Release(self->specifiers->data[i]);
	for (size_t i = 0; i < self->qualifiers->size; i++)
		Release(self->qualifiers->data[i]);
	Release(self->specifiers);
	Release(self->qualifiers);
}
