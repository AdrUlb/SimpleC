#ifndef ARRAY_TYPE
#error "ARRAY_TYPE must be defined before including this file"
#define ARRAY_TYPE IntArray
#endif

#ifndef ARRAY_ELEMENT_TYPE
#error "ARRAY_ELEMENT_TYPE must be defined before including this file"
#define ARRAY_ELEMENT_TYPE int
#endif

#define ELEMENT_SIZE sizeof(ARRAY_ELEMENT_TYPE)
#define CONCAT(a, b) a##b
#define EXPAND_AND_CONCAT(a, b) CONCAT(a, b)
#define F(name, ...) EXPAND_AND_CONCAT(EXPAND_AND_CONCAT(ARRAY_TYPE, _), name)(__VA_ARGS__)

#include <stddef.h>
#include <stdlib.h>

typedef struct ARRAY_TYPE
{
	void* data;
	size_t length;
} ARRAY_TYPE;

static void F(Init, ARRAY_TYPE* self, const size_t length)
{
	self->data = malloc(length * ELEMENT_SIZE);
	self->length = length;
}

static void F(Fini, const ARRAY_TYPE* self)
{
	free(self->data);
}

#undef ELEMENT_SIZE
#undef CONCAT
#undef EXPAND_AND_CONCAT
#undef F
