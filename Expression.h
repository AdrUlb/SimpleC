#pragma once

#include "Token.h"
#include "Type.h"

#define EXPR_ENUM_VALUES \
	X(EXPR_UNARY) \
	X(EXPR_BINARY) \
	X(EXPR_TERNARY) \
	X(EXPR_CAST) \
	X(EXPR_SIZEOF_TYPE) \
	X(EXPR_MEMBER_ACCESS) \
	X(EXPR_CALL) \
	X(EXPR_PRIMARY)

#define UNOP_ENUM_VALUES \
	X(UNOP_NONE) \
	X(UNOP_SIZEOF) \
	X(UNOP_ADDRESS_OF) \
	X(UNOP_DEREFERENCE) \
	X(UNOP_PLUS) \
	X(UNOP_MINUS) \
	X(UNOP_BITWISE_NOT) \
	X(UNOP_LOGICAL_NOT) \
	X(UNOP_PRE_INCREMENT) \
	X(UNOP_PRE_DECREMENT) \
	X(UNOP_POST_INCREMENT) \
	X(UNOP_POST_DECREMENT)

#define BINOP_ENUM_VALUES \
	X(BINOP_NONE) \
	X(BINOP_COMMA) \
	X(BINOP_ASSIGN) \
	X(BINOP_ASSIGN_MULTIPLY) \
	X(BINOP_ASSIGN_DIVIDE) \
	X(BINOP_ASSIGN_MODULO) \
	X(BINOP_ASSIGN_ADD) \
	X(BINOP_ASSIGN_SUBTRACT) \
	X(BINOP_ASSIGN_LEFT_SHIFT) \
	X(BINOP_ASSIGN_RIGHT_SHIFT) \
	X(BINOP_ASSIGN_AND) \
	X(BINOP_ASSIGN_XOR) \
	X(BINOP_ASSIGN_OR) \
	X(BINOP_LOGICAL_OR) \
	X(BINOP_LOGICAL_AND) \
	X(BINOP_BITWISE_OR) \
	X(BINOP_BITWISE_XOR) \
	X(BINOP_BITWISE_AND) \
	X(BINOP_TEST_EQUAL) \
	X(BINOP_TEST_NOT_EQUAL) \
	X(BINOP_TEST_LESS) \
	X(BINOP_TEST_GREATER) \
	X(BINOP_TEST_LESS_EQUAL) \
	X(BINOP_TEST_GREATER_EQUAL) \
	X(BINOP_SHIFT_LEFT) \
	X(BINOP_SHIFT_RIGHT) \
	X(BINOP_ADD) \
	X(BINOP_SUBTRACT) \
	X(BINOP_MULTIPLY) \
	X(BINOP_DIVIDE) \
	X(BINOP_MODULO) \
	X(BINOP_SUBSCRIPT)

#define TERNOP_ENUM_VALUES \
	X(TERNOP_NONE) \
	X(TERNOP_CONDITIONAL)

#define X(name) name,

typedef enum
{
	EXPR_ENUM_VALUES
} Expression_Type;

typedef enum
{
	UNOP_ENUM_VALUES
} UnaryOperation;

typedef enum
{
	BINOP_ENUM_VALUES
} BinaryOperation;

typedef enum
{
	TERNOP_ENUM_VALUES
} TernaryOperation;

#undef X

static const char* Expression_Type_ToString(const Expression_Type type)
{
	switch (type)
	{
#define X(name) case name: return #name;
		EXPR_ENUM_VALUES
#undef X
		default:
			return "<unknown>";
	}
}

static const char* UnaryOperation_ToString(const UnaryOperation type)
{
	switch (type)
	{
#define X(name) case name: return #name;
		UNOP_ENUM_VALUES
#undef X
		default:
			return "<unknown>";
	}
}

static const char* BinaryOperation_ToString(const BinaryOperation type)
{
	switch (type)
	{
#define X(name) case name: return #name;
		BINOP_ENUM_VALUES
#undef X
		default:
			return "<unknown>";
	}
}

static const char* TernaryOperation_ToString(const TernaryOperation type)
{
	switch (type)
	{
#define X(name) case name: return #name;
		TERNOP_ENUM_VALUES
#undef X
		default:
			return "<unknown>";
	}
}

#undef EXPR_ENUM_VALUES
#undef UNOP_ENUM_VALUES
#undef BINOP_ENUM_VALUES
#undef TERNOP_ENUM_VALUES

typedef struct Expression Expression;

#define LIST_TYPE ExpressionList
#define LIST_ELEMENT_TYPE Expression*
#include "Util/ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

typedef struct
{
	UnaryOperation operation;
	Expression* expression;
} UnaryExpression;

typedef struct
{
	BinaryOperation operation;
	Expression* left;
	Expression* right;
} BinaryExpression;

typedef struct
{
	TernaryOperation operation;
	Expression* left;
	Expression* middle;
	Expression* right;
} TernaryExpression;

typedef struct
{
	Type* type;
	Expression* expression;
} CastExpression;

typedef struct
{
	Type* type;
} SizeofTypeExpression;

typedef struct
{
	Expression* expression;
	ConstCharSpan memberName;
	bool isPointerAccess;
} MemberAccessExpression;

typedef struct
{
	Expression* callee;
	ExpressionList* arguments;
} CallExpression;

typedef struct
{
	Token literal;
} PrimaryExpression;

typedef union Expression_Data
{
	UnaryExpression unary;
	BinaryExpression binary;
	TernaryExpression ternary;
	CastExpression cast;
	SizeofTypeExpression sizeofType;
	MemberAccessExpression memberAccess;
	CallExpression call;
	PrimaryExpression primary;
} Expression_Data;

struct Expression
{
	SourceLocation location;
	Expression_Type type;
	Expression_Data data;
};

static Expression* Expression_Init_WithUnary(Expression* self, const UnaryExpression data, const SourceLocation location)
{
	self->type = EXPR_UNARY;
	self->data.unary = data;
	self->location = location;
	return self;
}

static Expression* Expression_Init_WithBinary(Expression* self, const BinaryExpression data, const SourceLocation location)
{
	self->type = EXPR_BINARY;
	self->data.binary = data;
	self->location = location;
	return self;
}

static Expression* Expression_Init_WithTernary(Expression* self, const TernaryExpression data, const SourceLocation location)
{
	self->type = EXPR_TERNARY;
	self->data.ternary = data;
	self->location = location;
	return self;
}

static Expression* Expression_Init_WithCast(Expression* self, const CastExpression data, const SourceLocation location)
{
	self->type = EXPR_CAST;
	self->data.cast = data;
	self->location = location;
	return self;
}

static Expression* Expression_Init_WithSizeofType(Expression* self, const SizeofTypeExpression data, const SourceLocation location)
{
	self->type = EXPR_SIZEOF_TYPE;
	self->data.sizeofType = data;
	self->location = location;
	return self;
}

static Expression* Expression_Init_WithMemberAccess(Expression* self, const MemberAccessExpression data, const SourceLocation location)
{
	self->type = EXPR_MEMBER_ACCESS;
	self->data.memberAccess = data;
	self->location = location;
	return self;
}

static Expression* Expression_Init_WithCall(Expression* self, const CallExpression data, const SourceLocation location)
{
	self->type = EXPR_CALL;
	self->data.call = data;
	self->location = location;
	return self;
}

static Expression* Expression_Init_WithPrimary(Expression* self, const PrimaryExpression data, const SourceLocation location)
{
	self->type = EXPR_PRIMARY;
	self->data.primary = data;
	self->location = location;
	return self;
}

static void Expression_Fini(const Expression* self)
{
	switch (self->type)
	{
		case EXPR_UNARY:
			Release(self->data.unary.expression);
			break;
		case EXPR_BINARY:
			Release(self->data.binary.left);
			Release(self->data.binary.right);
			break;
		case EXPR_TERNARY:
			Release(self->data.ternary.left);
			Release(self->data.ternary.middle);
			Release(self->data.ternary.right);
			break;
		case EXPR_CAST:
			Release(self->data.cast.type);
			break;
		case EXPR_SIZEOF_TYPE:
			Release(self->data.sizeofType.type);
			break;
		case EXPR_MEMBER_ACCESS:
			Release(self->data.memberAccess.expression);
			break;
		case EXPR_CALL:
			Release(self->data.call.callee);
			for (size_t i = 0; i < self->data.call.arguments->size; i++)
				Release(self->data.call.arguments->data[i]);
			Release(self->data.call.arguments);
			break;
		default:
			break;
	}
}
