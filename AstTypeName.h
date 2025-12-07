#pragma once

#include "AstTypeSpecifierQualifierList.h"

typedef struct
{
	SourceLocation location;
	AstTypeSpecifierQualifierList* specifierQualifierList;
} AstTypeName;

static AstTypeName* AstTypeName_Init_WithArgs(AstTypeName* self, AstTypeSpecifierQualifierList* specifierQualifierList, const SourceLocation location)
{
	self->location = location;
	self->specifierQualifierList = specifierQualifierList;
	return self;
}

static void AstTypeName_Fini(const AstTypeName* self)
{
	Release(self->specifierQualifierList);
}
