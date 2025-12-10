#pragma once

#define AST_FUNCTIONSPECIFIERS_ENUM_VALUES \
	X(NONE, 0) \
	X(INLINE, 1 << 0)

typedef enum AstFunctionSpecifiers
{
#define X(name, value) AST_FUNCTIONSPECIFIERS_##name = value,
	AST_FUNCTIONSPECIFIERS_ENUM_VALUES
#undef X
} AstFunctionSpecifiers;

static String* AstFunctionSpecifiers_ToString(const AstFunctionSpecifiers type)
{
	String* str = New(String);

#define X(name, value) \
	if (type & AST_FUNCTIONSPECIFIERS_##name) \
	{ \
		if (String_Length(str) != 0) \
			String_AppendCString(str, ", "); \
		String_AppendCString(str, #name); \
	}

	AST_FUNCTIONSPECIFIERS_ENUM_VALUES
#undef X

	return str;
}
