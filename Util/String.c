#include "String.h"

#include <string.h>

void String_Init(String* str)
{
	String_Init_With_Capacity(str, 0);
}

void String_Init_With_CString(String* str, const char* cstr)
{
	const size_t len = cstr == NULL ? 0 : strlen(cstr);
	String_Init_With_Capacity(str, len);
	String_Resize(str, len);

	char* buffer = String_GetBuffer(str);
	strncpy(buffer, cstr, len);
	buffer[len] = '\0';
}

void String_Init_With_Capacity(String* str, const size_t capacity)
{
	if (capacity > STRING_SHORT_CAPACITY__)
	{
		str->long__.data = malloc(capacity + 1);
		str->long__.data[0] = '\0';
		str->long__.capacity = capacity;

		str->isLong__ = true;
		str->length__ = 0;
		return;
	}

	str->short__.data[0] = '\0';
	str->isLong__ = false;
	str->length__ = 0;
}

void String_Fini(const String* str)
{
	if (str->isLong__)
		free(str->long__.data);
}

size_t String_Length(const String* str)
{
	return str->length__;
}

void String_Resize(String* str, const size_t newLength)
{
	if (str->isLong__)
	{
		if (newLength <= str->long__.capacity)
		{
			str->length__ = newLength;
			return;
		}

		size_t newCapacity = str->long__.capacity;
		while (newCapacity < newLength)
			newCapacity = newCapacity + newCapacity / 2;

		str->long__.capacity = newCapacity;
		str->long__.data = realloc(str->long__.data, newCapacity + 1);
		str->long__.data[newLength] = '\0';

		str->length__ = newLength;
		return;
	}

	if (newLength <= STRING_SHORT_CAPACITY__)
	{
		str->length__ = newLength;
		return;
	}

	// Move to long string
	size_t newCapacity = STRING_SHORT_CAPACITY__;
	while (newCapacity < newLength)
		newCapacity = newCapacity + newCapacity / 2;

	char* newData = malloc(newCapacity + 1);
	strncpy(newData, str->short__.data, str->length__);
	newData[newLength] = '\0';

	str->long__.capacity = newCapacity;
	str->long__.data = newData;

	str->length__ = newLength;
	str->isLong__ = true;
}

void String_AppendChar(String* str, const char c)
{
	const size_t len = String_Length(str);
	String_Resize(str, len + 1);

	char* buffer = String_GetBuffer(str);
	buffer[len] = c;
	buffer[len + 1] = '\0';
}

void String_AppendCString(String* str, const char* strToAppend)
{
	if (strToAppend == NULL)
		return;

	const size_t len = String_Length(str);
	const size_t appendLen = strlen(strToAppend);
	String_Resize(str, len + appendLen);

	char* buffer = String_GetBuffer(str);
	strncpy(buffer, strToAppend, appendLen);
	buffer[len + appendLen] = '\0';
}

void String_AppendCodePoint(String* str, uint32_t codePoint)
{
restart:
	// ASCII
	if (codePoint < 0x80)
	{
		String_AppendChar(str, (char)codePoint);
		return;
	}

	// Two-byte UTF-8
	if (codePoint < 0x800)
	{
		String_AppendChar(str, (char)(0xC0 | (codePoint >> 6)));
		String_AppendChar(str, (char)(0x80 | (codePoint & 0x3F)));
		return;
	}

	// Invalid code points (surrogates)
	if (codePoint >= 0xD800 && codePoint < 0xE000)
		goto replacementChar;

	// Three-byte UTF-8
	if (codePoint < 0x10000)
	{
		String_AppendChar(str, (char)(0xE0 | (codePoint >> 12)));
		String_AppendChar(str, (char)(0x80 | ((codePoint >> 6) & 0x3F)));
		String_AppendChar(str, (char)(0x80 | (codePoint & 0x3F)));
		return;
	}

	// Four-byte UTF-8
	if (codePoint < 0x110000)
	{
		String_AppendChar(str, (char)(0xF0 | (codePoint >> 18)));
		String_AppendChar(str, (char)(0x80 | ((codePoint >> 12) & 0x3F)));
		String_AppendChar(str, (char)(0x80 | ((codePoint >> 6) & 0x3F)));
		String_AppendChar(str, (char)(0x80 | (codePoint & 0x3F)));
		return;
	}

replacementChar:
	codePoint = 0xFFFD; // Replacement character
	goto restart;;;
}

char* String_GetBuffer(String* str)
{
	if (str == NULL)
		return "";

	return str->isLong__ ? str->long__.data : str->short__.data;
}

const char* String_AsCString(const String* str)
{
	if (str == NULL)
		return "";

	return str->isLong__ ? str->long__.data : str->short__.data;
}

CharSpan String_AsCharSpan(String* str)
{
	CharSpan span;
	CharSpan_Init(&span, String_GetBuffer(str), String_Length(str));
	return span;
}

ConstCharSpan String_AsConstCharSpan(const String* str)
{
	ConstCharSpan span;
	ConstCharSpan_Init(&span, String_AsCString(str), String_Length(str));
	return span;
}
