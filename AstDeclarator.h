#pragma once
#include "AstDirectDeclarator.h"
#include "AstPointer.h"

typedef struct AstDeclarator
{
	AstPointer* pointer_opt;
	AstDirectDeclarator* directDeclarator;
	SourceLocation location;
} AstDeclarator;

static AstDeclarator* AstDeclarator_Init_WithArgs(AstDeclarator* self, AstPointer* pointer_opt, AstDirectDeclarator* directDeclarator, const SourceLocation location)
{
	self->location = location;
	self->pointer_opt = pointer_opt;
	self->directDeclarator = directDeclarator;
	return self;
}

static void AstDeclarator_Fini(const AstDeclarator* self)
{
	Release(self->pointer_opt);
	Release(self->directDeclarator);
}