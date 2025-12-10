#pragma once

#include "AstTypeQualifier.h"
#include "AstTypeSpecifier.h"
#include "SourceFile.h"

typedef struct
{
	SourceLocation location;
	AstTypeSpecifierList* specifiers;
	AstTypeQualifiers qualifiers;
} AstTypeSpecifierQualifierList;

static AstTypeSpecifierQualifierList* AstTypeSpecifierQualifierList_Init_WithArgs(AstTypeSpecifierQualifierList* self,
                                                                                  AstTypeSpecifierList* specifiers,
                                                                                  const AstTypeQualifiers qualifiers,
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
	Release(self->specifiers);
}
