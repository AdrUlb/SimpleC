#pragma once

#include <stdint.h>

#define ARRAY_TYPE CharArray
#define ARRAY_ELEMENT_TYPE char
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE ShortArray
#define ARRAY_ELEMENT_TYPE short
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE IntArray
#define ARRAY_ELEMENT_TYPE int
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE LongArray
#define ARRAY_ELEMENT_TYPE long
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE LongLongArray
#define ARRAY_ELEMENT_TYPE long long
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE UCharArray
#define ARRAY_ELEMENT_TYPE unsigned char
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE UShortArray
#define ARRAY_ELEMENT_TYPE unsigned short
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE UIntArray
#define ARRAY_ELEMENT_TYPE unsigned int
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE ULongArray
#define ARRAY_ELEMENT_TYPE unsigned long
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE ULongLongArray
#define ARRAY_ELEMENT_TYPE unsigned long long
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE Int8Array
#define ARRAY_ELEMENT_TYPE int8_t
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE Int16Array
#define ARRAY_ELEMENT_TYPE int16_t
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE Int32Array
#define ARRAY_ELEMENT_TYPE int32_t
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE Int64Array
#define ARRAY_ELEMENT_TYPE int64_t
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE UInt8Array
#define ARRAY_ELEMENT_TYPE uint8_t
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE UInt16Array
#define ARRAY_ELEMENT_TYPE uint16_t
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE UInt32Array
#define ARRAY_ELEMENT_TYPE uint32_t
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE UInt64Array
#define ARRAY_ELEMENT_TYPE uint64_t
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE SSizeArray
#define ARRAY_ELEMENT_TYPE ssize_t
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE SizeArray
#define ARRAY_ELEMENT_TYPE size_t
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE IntPtrArray
#define ARRAY_ELEMENT_TYPE intptr_t
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE

#define ARRAY_TYPE UIntPtrArray
#define ARRAY_ELEMENT_TYPE uintptr_t
#include "ArrayDef.h"
#undef ARRAY_TYPE
#undef ARRAY_ELEMENT_TYPE
