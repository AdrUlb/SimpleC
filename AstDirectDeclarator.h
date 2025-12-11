#pragma once

#include "SourceFile.h"

nullable_begin

#define AST_DIRECTDECLARATOR_ENUM_VALUES \
	X(NONE) \
	X(IDENTIFIER) \
	X(PARENTHESIZED)

typedef enum AstDirectDeclarator_Type
{
#define X(name) AST_DIRECTDECLARATOR_##name,
	AST_DIRECTDECLARATOR_ENUM_VALUES
#undef X
} AstDirectDeclarator_Type;

static const char* AstDirectDeclarator_Type_ToString(const AstDirectDeclarator_Type type)
{
	switch (type)
	{
#define X(name) case AST_DIRECTDECLARATOR_##name: return #name;
		AST_DIRECTDECLARATOR_ENUM_VALUES
#undef X
		default:
			return "<unknown>";
	}
}

typedef struct AstDeclarator AstDeclarator;

typedef struct
{
	SourceLocation location;
	AstDirectDeclarator_Type type;

	union
	{
		ConstCharSpan identifier;
		AstDeclarator* parenthesized;
	};
} AstDirectDeclarator;

static AstDirectDeclarator* AstDirectDeclarator_Init_WithIdentifier(AstDirectDeclarator* self,
                                                                    const ConstCharSpan identifier,
                                                                    const SourceLocation location)
{
	self->type = AST_DIRECTDECLARATOR_IDENTIFIER;
	self->identifier = identifier;
	self->location = location;
	return self;
}

static AstDirectDeclarator* AstDirectDeclarator_Init_WithParenthesized(AstDirectDeclarator* self,
                                                                       AstDeclarator* parenthesized,
                                                                       const SourceLocation location)
{
	self->type = AST_DIRECTDECLARATOR_IDENTIFIER;
	self->parenthesized = parenthesized;
	self->location = location;
	return self;
}

static void AstDirectDeclarator_Fini(const AstDirectDeclarator* self)
{
	switch (self->type)
	{
		case AST_DIRECTDECLARATOR_PARENTHESIZED:
			Release(self->parenthesized);
			break;
		case AST_DIRECTDECLARATOR_NONE:
		case AST_DIRECTDECLARATOR_IDENTIFIER:
		default:
			break;
	}
}

nullable_end
