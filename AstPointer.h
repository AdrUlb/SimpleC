#pragma once
#include "AstTypeQualifier.h"

nullable_begin

typedef struct
{
	SourceLocation location;
	AstTypeQualifiers qualifiers;
} AstPointer;

static AstPointer* AstPointer_Init_WithArgs(AstPointer* self, AstTypeQualifiers qualifiers, SourceLocation location)
{
	self->location = location;
	self->qualifiers = qualifiers;
	return self;
}

static void AstPointer_Fini(const AstPointer* self)
{
	(void)self;
}

nullable_end
