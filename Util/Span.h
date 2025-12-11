#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#define DEFINE_SPAN(spanType, spanElementType) \
	typedef struct spanType \
	{ \
		size_t length; \
		spanElementType* data; \
	} spanType; \
	\
	static spanType spanType##_##Create(spanElementType* data, const size_t length) \
	{ \
		return (spanType) { \
			.data = data, \
			.length = length, \
		}; \
	} \
	\
	static spanType spanType##_##SubSpan(const spanType self, const size_t offset, const size_t length) \
	{ \
		assert(length == 0 || (offset < self.length && offset + length <= self.length)); \
		return spanType##_##Create(self.data + offset, length); \
	} \
	\
	static const spanType spanType##_Empty = { 0, NULL };

#define DEFINE_SPANS(spanType, spanElementType) \
	DEFINE_SPAN(spanType, spanElementType) \
	DEFINE_SPAN(Const##spanType, const spanElementType) \
	DEFINE_SPAN(U##spanType, unsigned spanElementType) \
	DEFINE_SPAN(ConstU##spanType, const unsigned spanElementType) \

#define Span_AsFormat(span) ((int)(span)->length), (span)->data

//#include "SpanDef.h"
DEFINE_SPANS(CharSpan, char)
DEFINE_SPANS(ShortSpan, short)
DEFINE_SPANS(IntSpan, int)
DEFINE_SPANS(LongSpan, long)
DEFINE_SPANS(LongLongSpan, long long)

DEFINE_SPAN(Int8Span, int8_t)
DEFINE_SPAN(Int16Span, int16_t)
DEFINE_SPAN(Int32Span, int32_t)
DEFINE_SPAN(Int64Span, int64_t)
DEFINE_SPAN(UInt8Span, int8_t)
DEFINE_SPAN(UInt16Span, int16_t)
DEFINE_SPAN(UInt32Span, int32_t)
DEFINE_SPAN(UInt64Span, int64_t)
DEFINE_SPAN(SSizeSpan, ssize_t)
DEFINE_SPAN(SizeSpan, size_t)
DEFINE_SPAN(IntPtrSpan, intptr_t)

DEFINE_SPANS(CStringSpan, char*)
