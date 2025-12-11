#pragma once
#include "SourceFile.h"
#include "Util/Span.h"

nullable_begin

#define TOKEN_ENUM_VALUES \
	X(EOF) \
	X(UNEXPECTED) \
	X(WHITESPACE) \
	X(COMMENT_SINGLELINE) \
	X(COMMENT_MULTILINE) \
	\
	X(LITERAL_INTEGER) \
	X(LITERAL_FLOAT) \
	X(LITERAL_CHAR) \
	X(LITERAL_STRING) \
	\
	X(IDENTIFIER) \
	\
	\
	X(KEYWORD_CHAR) \
	X(KEYWORD_DOUBLE) \
	X(KEYWORD_FLOAT) \
	X(KEYWORD_INT) \
	X(KEYWORD_LONG) \
	X(KEYWORD_SHORT) \
	X(KEYWORD_VOID) \
	\
	X(KEYWORD_CONST) \
	X(KEYWORD_VOLATILE) \
	X(KEYWORD_SIGNED) \
	X(KEYWORD_UNSIGNED) \
	X(KEYWORD_RESTRICT) \
	\
	X(KEYWORD_AUTO) \
	X(KEYWORD_EXTERN) \
	X(KEYWORD_REGISTER) \
	X(KEYWORD_STATIC) \
	X(KEYWORD_TYPEDEF) \
	\
	X(KEYWORD_INLINE) \
	\
	X(KEYWORD_STRUCT) \
	X(KEYWORD_UNION) \
	X(KEYWORD_ENUM) \
	\
	X(KEYWORD_IF) \
	X(KEYWORD_ELSE) \
	X(KEYWORD_SWITCH) \
	X(KEYWORD_CASE) \
	X(KEYWORD_DEFAULT) \
	\
	X(KEYWORD_FOR) \
	X(KEYWORD_DO) \
	X(KEYWORD_WHILE) \
	X(KEYWORD_BREAK) \
	X(KEYWORD_CONTINUE) \
	X(KEYWORD_GOTO) \
	X(KEYWORD_RETURN) \
	\
	X(KEYWORD_SIZEOF) \
	\
	X(KEYWORD_ALIGNAS) \
	X(KEYWORD_ALIGNOF) \
	X(KEYWORD_ATOMIC) \
	X(KEYWORD_GENERIC) \
	X(KEYWORD_STATICASSERT) \
	X(KEYWORD_THREADLOCAL) \
	\
	X(PUNCTUATOR_EQUAL)  \
	X(PUNCTUATOR_PLUS) \
	X(PUNCTUATOR_MINUS) \
	X(PUNCTUATOR_ASTERISK) \
	X(PUNCTUATOR_SLASH) \
	X(PUNCTUATOR_PERCENT) \
	X(PUNCTUATOR_AMPERSAND) \
	X(PUNCTUATOR_PIPE) \
	X(PUNCTUATOR_CARET) \
	X(PUNCTUATOR_TILDE) \
	X(PUNCTUATOR_EXCLAMATION) \
	X(PUNCTUATOR_QUESTION) \
	X(PUNCTUATOR_COLON) \
	X(PUNCTUATOR_HASH) \
	X(PUNCTUATOR_LESS) \
	X(PUNCTUATOR_GREATER) \
	X(PUNCTUATOR_PERIOD) \
	X(PUNCTUATOR_COMMA) \
	X(PUNCTUATOR_SEMICOLON) \
	X(PUNCTUATOR_PARENOPEN) \
	X(PUNCTUATOR_PARENCLOSE) \
	X(PUNCTUATOR_BRACEOPEN) \
	X(PUNCTUATOR_BRACECLOSE) \
	X(PUNCTUATOR_BRACKETOPEN) \
	X(PUNCTUATOR_BRACKETCLOSE) \
	X(PUNCTUATOR_DOUBLEQUOTE) \
	X(PUNCTUATOR_SINGLEQUOTE) \
	\
	X(PUNCTUATOR_EQUAL_EQUAL) \
	X(PUNCTUATOR_EXCLAMATION_EQUAL) \
	X(PUNCTUATOR_LESS_EQUAL) \
	X(PUNCTUATOR_GREATER_EQUAL) \
	X(PUNCTUATOR_PLUS_PLUS) \
	X(PUNCTUATOR_MINUS_MINUS) \
	X(PUNCTUATOR_PLUS_EQUAL) \
	X(PUNCTUATOR_MINUS_EQUAL) \
	X(PUNCTUATOR_ASTERISK_EQUAL) \
	X(PUNCTUATOR_SLASH_EQUAL) \
	X(PUNCTUATOR_PERCENT_EQUAL) \
	X(PUNCTUATOR_AMPERSAND_EQUAL) \
	X(PUNCTUATOR_PIPE_EQUAL) \
	X(PUNCTUATOR_CARET_EQUAL) \
	X(PUNCTUATOR_AMPERSAND_AMPERSAND) \
	X(PUNCTUATOR_PIPE_PIPE) \
	X(PUNCTUATOR_LESS_LESS) \
	X(PUNCTUATOR_GREATER_GREATER) \
	X(PUNCTUATOR_MINUS_GREATER) \
	\
	X(PUNCTUATOR_LESS_LESS_EQUAL) \
	X(PUNCTUATOR_GREATER_GREATER_EQUAL) \
	\
	X(MAX)

typedef enum
{
#define X(name) TOKEN_##name,
	TOKEN_ENUM_VALUES
#undef X
} Token_Type;

static const char* Token_Type_ToString(const Token_Type type)
{
	switch (type)
	{
#define X(name) case TOKEN_##name: return #name;
		TOKEN_ENUM_VALUES
#undef X
		default:
			return "<unknown>";
	}
}

#undef TOKEN_ENUM_VALUES

typedef struct
{
	const char* str;
	const Token_Type type;
} TokenStringMapEntry;

#define FLOATING_POINT_EXONENT_CHAR(isHex) ((isHex) ? 'p' : 'e')

static const TokenStringMapEntry punctuators[] = {
	// Three character punctuators
	{ "<<=", TOKEN_PUNCTUATOR_LESS_LESS_EQUAL },
	{ ">>=", TOKEN_PUNCTUATOR_GREATER_GREATER_EQUAL },
	//3,  Two character punctuators
	{ "==", TOKEN_PUNCTUATOR_EQUAL_EQUAL },
	{ "!=", TOKEN_PUNCTUATOR_EXCLAMATION_EQUAL },
	{ "<=", TOKEN_PUNCTUATOR_LESS_EQUAL },
	{ ">=", TOKEN_PUNCTUATOR_GREATER_EQUAL },
	{ "++", TOKEN_PUNCTUATOR_PLUS_PLUS },
	{ "--", TOKEN_PUNCTUATOR_MINUS_MINUS },
	{ "+=", TOKEN_PUNCTUATOR_PLUS_EQUAL },
	{ "-=", TOKEN_PUNCTUATOR_MINUS_EQUAL },
	{ "*=", TOKEN_PUNCTUATOR_ASTERISK_EQUAL },
	{ "/=", TOKEN_PUNCTUATOR_SLASH_EQUAL },
	{ "%=", TOKEN_PUNCTUATOR_PERCENT_EQUAL },
	{ "&=", TOKEN_PUNCTUATOR_AMPERSAND_EQUAL },
	{ "|=", TOKEN_PUNCTUATOR_PIPE_EQUAL },
	{ "^=", TOKEN_PUNCTUATOR_CARET_EQUAL },
	{ "&&", TOKEN_PUNCTUATOR_AMPERSAND_AMPERSAND },
	{ "||", TOKEN_PUNCTUATOR_PIPE_PIPE },
	{ "<<", TOKEN_PUNCTUATOR_LESS_LESS },
	{ ">>", TOKEN_PUNCTUATOR_GREATER_GREATER },
	{ "->", TOKEN_PUNCTUATOR_MINUS_GREATER },
	// Single character punctuators
	{ "=", TOKEN_PUNCTUATOR_EQUAL },
	{ "+", TOKEN_PUNCTUATOR_PLUS },
	{ "-", TOKEN_PUNCTUATOR_MINUS },
	{ "*", TOKEN_PUNCTUATOR_ASTERISK },
	{ "/", TOKEN_PUNCTUATOR_SLASH },
	{ "%", TOKEN_PUNCTUATOR_PERCENT },
	{ "&", TOKEN_PUNCTUATOR_AMPERSAND },
	{ "|", TOKEN_PUNCTUATOR_PIPE },
	{ "^", TOKEN_PUNCTUATOR_CARET },
	{ "~", TOKEN_PUNCTUATOR_TILDE },
	{ "!", TOKEN_PUNCTUATOR_EXCLAMATION },
	{ "?", TOKEN_PUNCTUATOR_QUESTION },
	{ ":", TOKEN_PUNCTUATOR_COLON },
	{ "#", TOKEN_PUNCTUATOR_HASH },
	{ "<", TOKEN_PUNCTUATOR_LESS },
	{ ">", TOKEN_PUNCTUATOR_GREATER },
	{ ".", TOKEN_PUNCTUATOR_PERIOD },
	{ ",", TOKEN_PUNCTUATOR_COMMA },
	{ ";", TOKEN_PUNCTUATOR_SEMICOLON },
	{ "(", TOKEN_PUNCTUATOR_PARENOPEN },
	{ ")", TOKEN_PUNCTUATOR_PARENCLOSE },
	{ "{", TOKEN_PUNCTUATOR_BRACEOPEN },
	{ "}", TOKEN_PUNCTUATOR_BRACECLOSE },
	{ "[", TOKEN_PUNCTUATOR_BRACKETOPEN },
	{ "]", TOKEN_PUNCTUATOR_BRACKETCLOSE },
	{ "\"", TOKEN_PUNCTUATOR_DOUBLEQUOTE },
	{ "\'", TOKEN_PUNCTUATOR_SINGLEQUOTE },
};

static const TokenStringMapEntry keywords[] = {

	// Keywords: types
	{ "char", TOKEN_KEYWORD_CHAR },
	{ "double", TOKEN_KEYWORD_DOUBLE },
	{ "float", TOKEN_KEYWORD_FLOAT },
	{ "int", TOKEN_KEYWORD_INT },
	{ "long", TOKEN_KEYWORD_LONG },
	{ "short", TOKEN_KEYWORD_SHORT },
	{ "void", TOKEN_KEYWORD_VOID },

	// Keywords: qualifiers
	{ "const", TOKEN_KEYWORD_CONST },
	{ "volatile", TOKEN_KEYWORD_VOLATILE },
	{ "signed", TOKEN_KEYWORD_SIGNED },
	{ "unsigned", TOKEN_KEYWORD_UNSIGNED },
	{ "restrict", TOKEN_KEYWORD_RESTRICT },

	// Keywords: storage classes
	{ "auto", TOKEN_KEYWORD_AUTO },
	{ "extern", TOKEN_KEYWORD_EXTERN },
	{ "register", TOKEN_KEYWORD_REGISTER },
	{ "static", TOKEN_KEYWORD_STATIC },
	{ "typedef", TOKEN_KEYWORD_TYPEDEF },

	// Keywords: function specifiers
	{ "inline", TOKEN_KEYWORD_INLINE },

	// Keywords: structures
	{ "struct", TOKEN_KEYWORD_STRUCT },
	{ "union", TOKEN_KEYWORD_UNION },
	{ "enum", TOKEN_KEYWORD_ENUM },

	// Keywords: control flow
	{ "if", TOKEN_KEYWORD_IF },
	{ "else", TOKEN_KEYWORD_ELSE },
	{ "switch", TOKEN_KEYWORD_SWITCH },
	{ "case", TOKEN_KEYWORD_CASE },
	{ "default", TOKEN_KEYWORD_DEFAULT },

	// Keywords: loops/jumps
	{ "for", TOKEN_KEYWORD_FOR },
	{ "do", TOKEN_KEYWORD_DO },
	{ "while", TOKEN_KEYWORD_WHILE },
	{ "break", TOKEN_KEYWORD_BREAK },
	{ "continue", TOKEN_KEYWORD_CONTINUE },
	{ "goto", TOKEN_KEYWORD_GOTO },
	{ "return", TOKEN_KEYWORD_RETURN },

	// Keywords: operators
	{ "sizeof", TOKEN_KEYWORD_SIZEOF },

	// Keywords: others
	{ "_Alignas", TOKEN_KEYWORD_ALIGNAS },
	{ "_Alignof", TOKEN_KEYWORD_ALIGNOF },
	{ "_Atomic", TOKEN_KEYWORD_ATOMIC },
	{ "_Generic", TOKEN_KEYWORD_GENERIC },
	{ "_Static_assert", TOKEN_KEYWORD_STATICASSERT },
	{ "_Thread_local", TOKEN_KEYWORD_THREADLOCAL },
	// New spellings as of C23
	{ "alignas", TOKEN_KEYWORD_ALIGNAS },
	{ "alignof", TOKEN_KEYWORD_ALIGNOF },
	{ "static_assert", TOKEN_KEYWORD_STATICASSERT },
	{ "thread_local", TOKEN_KEYWORD_THREADLOCAL },
};

typedef enum
{
	TOKEN_LITERAL_FLOAT_TYPE_DOUBLE,
	TOKEN_LITERAL_FLOAT_TYPE_FLOAT,
	TOKEN_LITERAL_FLOAT_TYPE_LONGDOUBLE,
	TOKEN_LITERAL_FLOAT_TYPE_BINARY16,
	TOKEN_LITERAL_FLOAT_TYPE_BINARY32,
	TOKEN_LITERAL_FLOAT_TYPE_BINARY64,
	TOKEN_LITERAL_FLOAT_TYPE_DECIMAL32,
	TOKEN_LITERAL_FLOAT_TYPE_DECIMAL64,
	TOKEN_LITERAL_FLOAT_TYPE_DECIMAL128,
} Token_LiteralFloat_Type;

typedef enum
{
	TOKEN_LITERAL_INTEGER_TYPE_INT,
	TOKEN_LITERAL_INTEGER_TYPE_LONG,
	TOKEN_LITERAL_INTEGER_TYPE_LONGLONG,
	TOKEN_LITERAL_INTEGER_TYPE_UNSIGNEDINT,
	TOKEN_LITERAL_INTEGER_TYPE_UNSIGNEDLONG,
	TOKEN_LITERAL_INTEGER_TYPE_UNSIGNEDLONGLONG,
} Token_LiteralInteger_Type;

typedef struct
{
	ConstCharSpan value;
	size_t base;
	Token_LiteralInteger_Type type;
} Token_LiteralInteger;

typedef struct
{
	bool isHex;

	bool hasIntegerPart;
	ConstCharSpan integerPart;

	bool hasFractionalPart;
	ConstCharSpan fractionalPart;

	bool hasExponent;
	bool exponentIsNegative;
	ConstCharSpan exponentPart;

	Token_LiteralFloat_Type type;
} Token_LiteralFloat;

typedef struct
{
	ConstCharSpan value;
} Token_LiteralIdentifier;

typedef union
{
	Token_LiteralInteger literalInteger;
	Token_LiteralFloat literalDecimalFloat;
	Token_LiteralIdentifier literalIdentifier;
} Token_Data;

typedef struct
{
	Token_Type type;
	SourceLocation location;
	Token_Data data;
} Token;

static Token Token_Create(const Token_Type type, const SourceLocation location, const Token_Data data)
{
	return (Token) {
		.type = type,
		.location = location,
		.data = data,
	};
}

void Token_Print(const Token* token);
String* Token_LiteralString_GetValue(const Token* token);

nullable_end

#define LIST_TYPE TokenList
#define LIST_ELEMENT_TYPE Token
#include "Util/ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE
