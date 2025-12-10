#pragma once
#include "AstDeclarationSpecifiers.h"

typedef struct
{
	AstDeclarationSpecifiers* declSpecs;
	SourceLocation location;
} AstDeclaration;

static AstDeclaration* AstDeclaration_Init_WithArgs(
	AstDeclaration* self,
	AstDeclarationSpecifiers* declSpecs,
	const SourceLocation location)
{
	self->declSpecs = declSpecs;
	self->location = location;
	return self;
}

static void AstDeclaration_Fini(const AstDeclaration* self)
{
	Release(self->declSpecs);
}
