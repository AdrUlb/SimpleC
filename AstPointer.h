#pragma once
#include "AstTypeQualifier.h"

typedef struct
{
	SourceLocation location;
	AstTypeQualifierList* qualifiers;
} AstPointer;

static AstPointer* AstPointer_Init_WithArgs(AstPointer* self, AstTypeQualifierList* qualifiers, SourceLocation location)
{
	self->location = location;
	self->qualifiers = qualifiers;
	return self;
}

static void AstPointer_Fini(const AstPointer* self)
{
	Release(self->qualifiers);
}