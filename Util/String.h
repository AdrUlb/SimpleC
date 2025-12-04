#pragma once

#include "List.h"
#include "Span.h"

#define STRING_SHORT_CAPACITY__ 15

typedef struct
{
	size_t capacity;
	char* data;
} String_Long__;

typedef struct
{
	char data[STRING_SHORT_CAPACITY__ + 1];
} String_Short__;

typedef struct
{
	union
	{
		String_Long__ long__;
		String_Short__ short__;
	};

	size_t length__;
	bool isLong__;
} String;

void String_Init(String* str);
void String_Init_With_CString(String* str, const char* cstr);
void String_Init_With_Capacity(String* str, size_t capacity);
void String_Fini(const String* str);
size_t String_Length(const String* str);
void String_Resize(String* str, size_t newLength);
void String_AppendChar(String* str, char c);
void String_AppendCString(String* str, const char* strToAppend);
void String_AppendCodePoint(String* str, uint32_t codePoint);
char* String_GetBuffer(String* str);
const char* String_AsCString(const String* str);
CharSpan String_AsCharSpan(String* str);
ConstCharSpan String_AsConstCharSpan(const String* str);
