#pragma once

#include <stddef.h>
#include <stdint.h>

#define SPAN_TYPE CharSpan
#define SPAN_ELEMENT_TYPE char
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ShortSpan
#define SPAN_ELEMENT_TYPE short
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE IntSpan
#define SPAN_ELEMENT_TYPE int
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE LongSpan
#define SPAN_ELEMENT_TYPE long
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE LongLongSpan
#define SPAN_ELEMENT_TYPE long long
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE UCharSpan
#define SPAN_ELEMENT_TYPE unsigned char
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE UShortSpan
#define SPAN_ELEMENT_TYPE unsigned short
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE UIntSpan
#define SPAN_ELEMENT_TYPE unsigned int
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ULongSpan
#define SPAN_ELEMENT_TYPE unsigned long
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ULongLongSpan
#define SPAN_ELEMENT_TYPE unsigned long long
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE Int8Span
#define SPAN_ELEMENT_TYPE int8_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE Int16Span
#define SPAN_ELEMENT_TYPE int16_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE Int32Span
#define SPAN_ELEMENT_TYPE int32_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE Int64Span
#define SPAN_ELEMENT_TYPE int64_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE UInt8Span
#define SPAN_ELEMENT_TYPE uint8_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE UInt16Span
#define SPAN_ELEMENT_TYPE uint16_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE UInt32Span
#define SPAN_ELEMENT_TYPE uint32_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE UInt64Span
#define SPAN_ELEMENT_TYPE uint64_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE SSizeSpan
#define SPAN_ELEMENT_TYPE ssize_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE SizeSpan
#define SPAN_ELEMENT_TYPE size_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE IntPtrSpan
#define SPAN_ELEMENT_TYPE intptr_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE UIntPtrSpan
#define SPAN_ELEMENT_TYPE uintptr_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstCharSpan
#define SPAN_ELEMENT_TYPE const char
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstShortSpan
#define SPAN_ELEMENT_TYPE const short
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstIntSpan
#define SPAN_ELEMENT_TYPE const int
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstLongSpan
#define SPAN_ELEMENT_TYPE const long
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstLongLongSpan
#define SPAN_ELEMENT_TYPE const long long
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstUCharSpan
#define SPAN_ELEMENT_TYPE const unsigned char
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstUShortSpan
#define SPAN_ELEMENT_TYPE const unsigned short
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstUIntSpan
#define SPAN_ELEMENT_TYPE const unsigned int
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstULongSpan
#define SPAN_ELEMENT_TYPE const unsigned long
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstULongLongSpan
#define SPAN_ELEMENT_TYPE const unsigned long long
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstInt8Span
#define SPAN_ELEMENT_TYPE const int8_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstInt16Span
#define SPAN_ELEMENT_TYPE const int16_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstInt32Span
#define SPAN_ELEMENT_TYPE const int32_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstInt64Span
#define SPAN_ELEMENT_TYPE const int64_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstUInt8Span
#define SPAN_ELEMENT_TYPE const uint8_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstUInt16Span
#define SPAN_ELEMENT_TYPE const uint16_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstUInt32Span
#define SPAN_ELEMENT_TYPE const uint32_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstUInt64Span
#define SPAN_ELEMENT_TYPE const uint64_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstSSizeSpan
#define SPAN_ELEMENT_TYPE const ssize_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstSizeSpan
#define SPAN_ELEMENT_TYPE const size_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstIntPtrSpan
#define SPAN_ELEMENT_TYPE const intptr_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE ConstUIntPtrSpan
#define SPAN_ELEMENT_TYPE const uintptr_t
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE

#define SPAN_TYPE CStringSpan
#define SPAN_ELEMENT_TYPE char*
#include "SpanDef.h"
#undef SPAN_TYPE
#undef SPAN_ELEMENT_TYPE
