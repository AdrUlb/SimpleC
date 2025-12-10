#pragma once
#include "AstFunctionSpecifier.h"
#include "AstStorageClassSpecifier.h"
#include "AstTypeQualifier.h"
#include "AstTypeSpecifier.h"
#include "Util/Managed.h"

typedef struct
{
	SourceLocation location;
	AstStorageClassSpecifierList* storageClassSpecifiers;
	AstTypeSpecifierList* typeSpecifiers;
	AstTypeQualifiers typeQualifiers;
	AstFunctionSpecifiers functionSpecifiers;
	// TODO: alignment specifiers
} AstDeclarationSpecifiers;

static AstDeclarationSpecifiers* AstDeclarationSpecifiers_Init_WithArgs(
	AstDeclarationSpecifiers* self,
	AstStorageClassSpecifierList* storageClassSpecifiers,
	AstTypeSpecifierList* typeSpecifiers,
	const AstTypeQualifiers typeQualifiers,
	const AstFunctionSpecifiers functionSpecifiers,
	const SourceLocation location)
{
	self->storageClassSpecifiers = storageClassSpecifiers;
	self->typeSpecifiers = typeSpecifiers;
	self->typeQualifiers = typeQualifiers;
	self->functionSpecifiers = functionSpecifiers;
	self->location = location;
	return self;
}

static void AstDeclarationSpecifiers_Fini(const AstDeclarationSpecifiers* self)
{
	Release(self->storageClassSpecifiers);
	Release(self->typeSpecifiers);
}
