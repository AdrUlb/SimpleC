#pragma once

#include <stdint.h>

#define LIST_TYPE CharList
#define LIST_ELEMENT_TYPE char
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE ShortList
#define LIST_ELEMENT_TYPE short
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE IntList
#define LIST_ELEMENT_TYPE int
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE LongList
#define LIST_ELEMENT_TYPE long
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE LongLongList
#define LIST_ELEMENT_TYPE long long
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE UCharList
#define LIST_ELEMENT_TYPE unsigned char
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE UShortList
#define LIST_ELEMENT_TYPE unsigned short
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE UIntList
#define LIST_ELEMENT_TYPE unsigned int
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE ULongList
#define LIST_ELEMENT_TYPE unsigned long
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE ULongLongList
#define LIST_ELEMENT_TYPE unsigned long long
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE Int8List
#define LIST_ELEMENT_TYPE int8_t
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE Int16List
#define LIST_ELEMENT_TYPE int16_t
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE Int32List
#define LIST_ELEMENT_TYPE int32_t
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE Int64List
#define LIST_ELEMENT_TYPE int64_t
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE UInt8List
#define LIST_ELEMENT_TYPE uint8_t
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE UInt16List
#define LIST_ELEMENT_TYPE uint16_t
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE UInt32List
#define LIST_ELEMENT_TYPE uint32_t
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE UInt64List
#define LIST_ELEMENT_TYPE uint64_t
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE SSizeList
#define LIST_ELEMENT_TYPE ssize_t
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE SizeList
#define LIST_ELEMENT_TYPE size_t
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE IntPtrList
#define LIST_ELEMENT_TYPE intptr_t
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE

#define LIST_TYPE UIntPtrList
#define LIST_ELEMENT_TYPE uintptr_t
#include "ListDef.h"
#undef LIST_TYPE
#undef LIST_ELEMENT_TYPE
