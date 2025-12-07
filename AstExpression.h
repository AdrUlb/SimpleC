#pragma once

#include "AstTypeName.h"
#include "Token.h"

#define AST_EXPR_ENUM_VALUES \
	X(UNARY) \
	X(BINARY) \
	X(TERNARY) \
	X(CAST) \
	X(SIZEOF_TYPE) \
	X(MEMBER_ACCESS) \
	X(CALL) \
	X(PRIMARY)

#define AST_UNOP_ENUM_VALUES \
	X(NONE) \
	X(SIZEOF) \
	X(ADDRESS_OF) \
	X(DEREFERENCE) \
	X(PLUS) \
	X(MINUS) \
	X(BITWISE_NOT) \
	X(LOGICAL_NOT) \
	X(PRE_INCREMENT) \
	X(PRE_DECREMENT) \
	X(POST_INCREMENT) \
	X(POST_DECREMENT)

#define AST_BINOP_ENUM_VALUES \
	X(NONE) \
	X(COMMA) \
	X(ASSIGN) \
	X(ASSIGN_MULTIPLY) \
	X(ASSIGN_DIVIDE) \
	X(ASSIGN_MODULO) \
	X(ASSIGN_ADD) \
	X(ASSIGN_SUBTRACT) \
	X(ASSIGN_LEFT_SHIFT) \
	X(ASSIGN_RIGHT_SHIFT) \
	X(ASSIGN_AND) \
	X(ASSIGN_XOR) \
	X(ASSIGN_OR) \
	X(LOGICAL_OR) \
	X(LOGICAL_AND) \
	X(BITWISE_OR) \
	X(BITWISE_XOR) \
	X(BITWISE_AND) \
	X(TEST_EQUAL) \
	X(TEST_NOT_EQUAL) \
	X(TEST_LESS) \
	X(TEST_GREATER) \
	X(TEST_LESS_EQUAL) \
	X(TEST_GREATER_EQUAL) \
	X(SHIFT_LEFT) \
	X(SHIFT_RIGHT) \
	X(ADD) \
	X(SUBTRACT) \
	X(MULTIPLY) \
	X(DIVIDE) \
	X(MODULO) \
	X(SUBSCRIPT)

#define AST_TERNOP_ENUM_VALUES \
	X(NONE) \
	X(CONDITIONAL)

typedef enum
{
#define X(name) AST_EXPR_##name,
	AST_EXPR_ENUM_VALUES
#undef X
} AstExpression_Type;

typedef enum
{
#define X(name) AST_UNOP_##name,
	AST_UNOP_ENUM_VALUES
#undef X
} AstUnaryOperation;

typedef enum
{
#define X(name) AST_BINOP_##name,
	AST_BINOP_ENUM_VALUES
#undef X
} AstBinaryOperation;

typedef enum
{
#define X(name) AST_TERNOP_##name,
	AST_TERNOP_ENUM_VALUES
#undef X
} AstTernaryOperation;

static const char* AstExpression_Type_ToString(const AstExpression_Type type)
{
	switch (type)
	{
#define X(name) case AST_EXPR_##name: return #name;
		AST_EXPR_ENUM_VALUES
#undef X
		default:
			return "<unknown>";
	}
}

static const char* AstUnaryOperation_ToString(const AstUnaryOperation type)
{
	switch (type)
	{
#define X(name) case AST_UNOP_##name: return #name;
		AST_UNOP_ENUM_VALUES
#undef X
		default:
			return "<unknown>";
	}
}

static const char* AstBinaryOperation_ToString(const AstBinaryOperation type)
{
	switch (type)
	{
#define X(name) case AST_BINOP_##name: return #name;
		AST_BINOP_ENUM_VALUES
#undef X
		default:
			return "<unknown>";
	}
}

static const char* AstTernaryOperation_ToString(const AstTernaryOperation type)
{
	switch (type)
	{
#define X(name) case AST_TERNOP_##name: return #name;
		AST_TERNOP_ENUM_VALUES
#undef X
		default:
			return "<unknown>";
	}
}

#undef AST_EXPR_ENUM_VALUES
#undef AST_UNOP_ENUM_VALUES
#undef AST_BINOP_ENUM_VALUES
#undef AST_TERNOP_ENUM_VALUES

typedef struct AstExpression AstExpression;

#define LIST_TYPE AstExpressionList
#define LIST_ELEMENT_TYPE AstExpression*
#include "Util/ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

typedef struct
{
	AstUnaryOperation operation;
	AstExpression* expression;
} AstUnaryExpression;

typedef struct
{
	AstBinaryOperation operation;
	AstExpression* left;
	AstExpression* right;
} AstBinaryExpression;

typedef struct
{
	AstTernaryOperation operation;
	AstExpression* left;
	AstExpression* middle;
	AstExpression* right;
} AstTernaryExpression;

typedef struct
{
	AstTypeName* typeName;
	AstExpression* expression;
} AstCastExpression;

typedef struct
{
	AstTypeName* typeName;
} AstSizeofTypeExpression;

typedef struct
{
	AstExpression* expression;
	ConstCharSpan memberName;
	bool isPointerAccess;
} AstMemberAccessExpression;

typedef struct
{
	AstExpression* callee;
	AstExpressionList* arguments;
} AstCallExpression;

typedef struct
{
	Token literal;
} AstPrimaryExpression;

typedef union
{
	AstUnaryExpression unary;
	AstBinaryExpression binary;
	AstTernaryExpression ternary;
	AstCastExpression cast;
	AstSizeofTypeExpression sizeofType;
	AstMemberAccessExpression memberAccess;
	AstCallExpression call;
	AstPrimaryExpression primary;
} AstExpression_Data;

struct AstExpression
{
	SourceLocation location;
	AstExpression_Type type;
	AstExpression_Data data;
};

static AstExpression* AstExpression_Init_WithUnary(AstExpression* self, const AstUnaryExpression data, const SourceLocation location)
{
	self->type = AST_EXPR_UNARY;
	self->data.unary = data;
	self->location = location;
	return self;
}

static AstExpression* AstExpression_Init_WithBinary(AstExpression* self, const AstBinaryExpression data, const SourceLocation location)
{
	self->type = AST_EXPR_BINARY;
	self->data.binary = data;
	self->location = location;
	return self;
}

static AstExpression* AstExpression_Init_WithTernary(AstExpression* self, const AstTernaryExpression data, const SourceLocation location)
{
	self->type = AST_EXPR_TERNARY;
	self->data.ternary = data;
	self->location = location;
	return self;
}

static AstExpression* AstExpression_Init_WithCast(AstExpression* self, const AstCastExpression data, const SourceLocation location)
{
	self->type = AST_EXPR_CAST;
	self->data.cast = data;
	self->location = location;
	return self;
}

static AstExpression* AstExpression_Init_WithSizeofType(AstExpression* self, const AstSizeofTypeExpression data, const SourceLocation location)
{
	self->type = AST_EXPR_SIZEOF_TYPE;
	self->data.sizeofType = data;
	self->location = location;
	return self;
}

static AstExpression* AstExpression_Init_WithMemberAccess(AstExpression* self, const AstMemberAccessExpression data, const SourceLocation location)
{
	self->type = AST_EXPR_MEMBER_ACCESS;
	self->data.memberAccess = data;
	self->location = location;
	return self;
}

static AstExpression* AstExpression_Init_WithCall(AstExpression* self, const AstCallExpression data, const SourceLocation location)
{
	self->type = AST_EXPR_CALL;
	self->data.call = data;
	self->location = location;
	return self;
}

static AstExpression* AstExpression_Init_WithPrimary(AstExpression* self, const AstPrimaryExpression data, const SourceLocation location)
{
	self->type = AST_EXPR_PRIMARY;
	self->data.primary = data;
	self->location = location;
	return self;
}

static void AstExpression_Fini(const AstExpression* self)
{
	switch (self->type)
	{
		case AST_EXPR_UNARY:
			Release(self->data.unary.expression);
			break;
		case AST_EXPR_BINARY:
			Release(self->data.binary.left);
			Release(self->data.binary.right);
			break;
		case AST_EXPR_TERNARY:
			Release(self->data.ternary.left);
			Release(self->data.ternary.middle);
			Release(self->data.ternary.right);
			break;
		case AST_EXPR_CAST:
			Release(self->data.cast.typeName);
			break;
		case AST_EXPR_SIZEOF_TYPE:
			Release(self->data.sizeofType.typeName);
			break;
		case AST_EXPR_MEMBER_ACCESS:
			Release(self->data.memberAccess.expression);
			break;
		case AST_EXPR_CALL:
			Release(self->data.call.callee);
			for (size_t i = 0; i < self->data.call.arguments->size; i++)
				Release(self->data.call.arguments->data[i]);
			Release(self->data.call.arguments);
			break;
		default:
			break;
	}
}
