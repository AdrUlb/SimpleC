#pragma once

#include <stdatomic.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct ManagedObjectHeader
{
	_Atomic size_t refCount;
	void (*fini)(void*);
} ManagedObjectHeader;

static void* NewImpl(const size_t size, void (*deleter)(void*))
{
	// Create header, also allocating space for the object
	ManagedObjectHeader* header = malloc(sizeof(ManagedObjectHeader) + size);
	if (header == NULL)
		abort();

	// Set header info
	atomic_store(&header->refCount, 1);
	header->fini = deleter;

	// Return pointer to object memory
	return header + 1;
}

static void ReleaseImpl(const void* ptr)
{
	if (ptr == NULL)
		return;

	// Get header
	ManagedObjectHeader* header = (ManagedObjectHeader*)ptr - 1;

	if (atomic_fetch_sub_explicit(&header->refCount, 1, memory_order_acq_rel) > 1)
		return;

	// Call deleter
	if (header->fini != NULL)
		header->fini((void*)ptr);

	// Free memory
	free(header);
}

static void* RetainImpl(void* ptr)
{
	if (ptr != NULL)
	{
		ManagedObjectHeader* header = ((ManagedObjectHeader*)ptr) - 1;
		atomic_fetch_add(&header->refCount, 1);
	}
	return ptr;
}

static const void* RetainConstImpl(const void* ptr)
{
	if (ptr != NULL)
	{
		ManagedObjectHeader* header = ((ManagedObjectHeader*)ptr) - 1;
		atomic_fetch_add(&header->refCount, 1);
	}
	return ptr;
}

static void CleanupUsingImpl(void* ptr)
{
	if (ptr == NULL)
		return;

	ptr = *(void**)ptr;
	ReleaseImpl(ptr);
}

#define using __attribute__((cleanup(CleanupUsingImpl)))
#define New(type) type##_Init(NewImpl(sizeof(type), (void (*)(void*))type##_Fini))
#define NewWith(type, with, ...) type##_Init_With##with(NewImpl(sizeof(type), (void (*)(void*))type##_Fini) __VA_OPT__(,) __VA_ARGS__)
#define Release(ptr) ReleaseImpl(ptr)
#define Retain(ptr) RetainImpl(ptr)
#define RetainConst(ptr) RetainConstImpl(ptr)
