#pragma once
#include "AstDirectDeclarator.h"
#include "AstPointer.h"

nullable_begin

typedef struct AstDeclarator
{
	AstPointer*nullable pointer;
	AstDirectDeclarator* directDeclarator;
	SourceLocation location;
} AstDeclarator;

static AstDeclarator* AstDeclarator_Init_WithArgs(AstDeclarator* self, AstPointer*nullable pointer, AstDirectDeclarator* directDeclarator, const SourceLocation location)
{
	self->location = location;
	self->pointer = pointer;
	self->directDeclarator = directDeclarator;
	return self;
}

static void AstDeclarator_Fini(const AstDeclarator* self)
{
	Release(self->pointer);
	Release(self->directDeclarator);
}

nullable_end
