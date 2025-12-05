#pragma once

#include "SourceFile.h"

typedef struct
{
	SourceLocation location;
} Type;

static Type* Type_Init(Type* self, const SourceLocation location)
{
	self->location = location;
	return self;
}

static void Type_Fini(const Type* self)
{
	(void)self;
}
