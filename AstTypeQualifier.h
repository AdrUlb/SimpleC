#pragma once

nullable_begin

#define AST_TYPEQUALIFIERS_ENUM_VALUES \
	X(NONE, 0) \
	X(CONST, 1 << 0) \
	X(RESTRICT, 1 << 1) \
	X(VOLATILE, 1 << 2)

typedef enum AstTypeQualifiers
{
#define X(name, value) AST_TYPEQUALIFIERS_##name = (value),
	AST_TYPEQUALIFIERS_ENUM_VALUES
#undef X
} AstTypeQualifiers;

static String* AstTypeQualifiers_ToString(const AstTypeQualifiers type)
{
	String* str = New(String);

#define X(name, value) \
	if (type & AST_TYPEQUALIFIERS_##name) \
	{ \
		if (String_Length(str) != 0) \
			String_AppendCString(str, ", "); \
		String_AppendCString(str, #name); \
	}

	AST_TYPEQUALIFIERS_ENUM_VALUES
#undef X

	return str;
}

nullable_end
