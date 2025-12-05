#include "String.h"

#include <string.h>

String* String_Init(String* self)
{
	String_Init_WithCapacity(self, 0);
	return self;
}

String* String_Init_WithCString(String* self, const char* cstr)
{
	const size_t len = cstr == NULL ? 0 : strlen(cstr);
	String_Init_WithCapacity(self, len);
	String_Resize(self, len);

	char* buffer = String_GetBuffer(self);
	strncpy(buffer, cstr, len);
	buffer[len] = '\0';

	return self;
}

String* String_Init_WithCapacity(String* self, const size_t capacity)
{
	if (capacity > STRING_SHORT_CAPACITY__)
	{
		self->long__.data = malloc(capacity + 1);
		self->long__.data[0] = '\0';
		self->long__.capacity = capacity;

		self->isLong__ = true;
		self->length = 0;
		return self;
	}

	self->short__.data[0] = '\0';
	self->isLong__ = false;
	self->length = 0;

	return self;
}

void String_Fini(const String* str)
{
	if (str->isLong__)
		free(str->long__.data);
}

size_t String_Length(const String* str)
{
	return str->length;
}

void String_Resize(String* str, const size_t newLength)
{
	if (str->isLong__)
	{
		if (newLength <= str->long__.capacity)
		{
			str->length = newLength;
			return;
		}

		size_t newCapacity = str->long__.capacity;
		while (newCapacity < newLength)
			newCapacity = newCapacity + newCapacity / 2;

		str->long__.capacity = newCapacity;
		str->long__.data = realloc(str->long__.data, newCapacity + 1);
		str->long__.data[newLength] = '\0';

		str->length = newLength;
		return;
	}

	if (newLength <= STRING_SHORT_CAPACITY__)
	{
		str->length = newLength;
		return;
	}

	// Move to long string
	size_t newCapacity = STRING_SHORT_CAPACITY__;
	while (newCapacity < newLength)
		newCapacity = newCapacity + newCapacity / 2;

	char* newData = malloc(newCapacity + 1);
	strncpy(newData, str->short__.data, str->length);
	newData[newLength] = '\0';

	str->long__.capacity = newCapacity;
	str->long__.data = newData;

	str->length = newLength;
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

void String_AppendConstCharSpan(String* str, const ConstCharSpan strToAppend)
{
	const size_t len = String_Length(str);
	String_Resize(str, len + strToAppend.length);

	char* buffer = String_GetBuffer(str);
	strncpy(buffer, strToAppend.data, strToAppend.length);
	buffer[len + strToAppend.length] = '\0';
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
	goto restart;
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
	return CharSpan_Create(String_GetBuffer(str), String_Length(str));
}

ConstCharSpan String_AsConstCharSpan(const String* str)
{
	return ConstCharSpan_Create(String_AsCString(str), String_Length(str));
}
