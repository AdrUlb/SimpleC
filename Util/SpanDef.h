#ifndef SPAN_TYPE
#error "SPAN_TYPE must be defined before including this file"
#endif

#ifndef SPAN_ELEMENT_TYPE
#error "SPAN_ELEMENT_TYPE must be defined before including this file"
#endif

#define ELEMENT_SIZE sizeof(SPAN_ELEMENT_TYPE)
#define CONCAT(a, b) a##b
#define EXPAND_AND_CONCAT(a, b) CONCAT(a, b)
#define F(name, ...) EXPAND_AND_CONCAT(EXPAND_AND_CONCAT(SPAN_TYPE, _), name)(__VA_ARGS__)
#define DEFINE_EMPTY_SPAN static const SPAN_TYPE EXPAND_AND_CONCAT(SPAN_TYPE, _Empty) = { NULL, 0, ELEMENT_SIZE };

#include <assert.h>
#include <stdio.h>

typedef struct SPAN_TYPE
{
	SPAN_ELEMENT_TYPE* data;
	size_t length;
	size_t elementSize;
} SPAN_TYPE;

static SPAN_TYPE* F(Init, SPAN_TYPE* self, SPAN_ELEMENT_TYPE* data, const size_t length)
{
	self->data = data;
	self->length = length;
	self->elementSize = ELEMENT_SIZE;
	return self;
}

static SPAN_TYPE F(SubSpan, const SPAN_TYPE self, const size_t offset, const size_t length)
{
	SPAN_TYPE newSpan;
	assert(length == 0 || (offset < self.length && offset + length <= self.length));
	F(Init, &newSpan, self.data + offset, length);
	return newSpan;
}

DEFINE_EMPTY_SPAN

#define Span_AsFormat(span) ((int)(span)->length), (span)->data

#undef ELEMENT_SIZE
#undef CONCAT
#undef EXPAND_AND_CONCAT
#undef F
