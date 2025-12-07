// ReSharper disable once CppMissingIncludeGuard
#include "Macros.h"

#ifndef LIST_TYPE
#error "LIST_TYPE_NAME must be defined before including this file"
#endif

#ifndef LIST_ELEMENT_TYPE
#error "LIST_ELEMENT_TYPE must be defined before including this file"
#endif

#define ELEMENT_SIZE sizeof(LIST_ELEMENT_TYPE)
#define F(name, ...) EXPAND_AND_CONCAT(EXPAND_AND_CONCAT(LIST_TYPE, _), name)(__VA_ARGS__)

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct LIST_TYPE
{
	LIST_ELEMENT_TYPE* data;
	size_t size;
	size_t capacity;
} LIST_TYPE;

static LIST_TYPE* F(Init_WithCapacity, LIST_TYPE* self, const size_t initialCapacity)
{
	if (self == NULL)
		return self;

	self->data = malloc(ELEMENT_SIZE * initialCapacity);
	if (self->data == NULL)
	{
		abort();
	}

	self->capacity = initialCapacity;
	self->size = 0;
	return self;
}

static LIST_TYPE* F(Init, LIST_TYPE* self)
{
	F(Init_WithCapacity, self, 16);
	return self;
}

static void F(Fini, const LIST_TYPE* self)
{
	if (self == NULL)
		return;

	free(self->data);
}

static bool F(Reserve, LIST_TYPE* self, const size_t newCapacity)
{
	if (self == NULL)
		return false;

	if (newCapacity <= self->capacity)
		return true;


	void* newData = realloc(self->data, ELEMENT_SIZE * newCapacity);
	if (newData == NULL)
		return false;

	self->data = newData;
	self->capacity = newCapacity;

	return true;
}

static bool F(Resize, LIST_TYPE* self, const size_t newSize)
{
	if (self == NULL)
		return false;

	if (newSize > self->capacity)
	{
		if (!F(Reserve, self, newSize))
			return false;
	}

	self->size = newSize;
	return true;
}

static bool F(AppendFromPtr, LIST_TYPE* self, const void* element)
{
	if (self == NULL || element == NULL)
		return false;

	// Grow if necessary
	if (self->size >= self->capacity)
	{
		const size_t newCapacity = self->capacity == 0 ? 16 : self->capacity + self->capacity / 2;
		if (!F(Reserve, self, newCapacity))
			return false;
	}

	const size_t index = self->size++;
	void* dest = self->data + index;
	memcpy(dest, element, ELEMENT_SIZE);
	return true;
}

static bool F(Append, LIST_TYPE* self, LIST_ELEMENT_TYPE const element)
{
	return F(AppendFromPtr, self, &element);
}

static bool F(RemoveAt, LIST_TYPE* self, const size_t index)
{
	if (self == NULL || index >= self->size)
		return false;

	if (index == self->size - 1)
	{
		// Removing the last element, no need to shift
		self->size--;
		return true;
	}

	void* dest = self->data + index;
	const void* src = self->data + index + 1;
	const size_t bytesToMove = self->size - index - 1;
	memmove(dest, src, bytesToMove);

	self->size--;
	return true;
}

#undef ELEMENT_SIZE
#undef F
