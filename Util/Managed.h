#pragma once

#include <stdlib.h>

typedef struct ManagedObjectHeader
{
	size_t refCount;
	void (*fini)(void*);
} ManagedObjectHeader;

static void* NewImpl(const size_t size, void (*deleter)(void*))
{
	// Create header, also allocating space for the object
	ManagedObjectHeader* header = malloc(sizeof(ManagedObjectHeader) + size);
	if (header == NULL)
		abort();

	// Set header info
	header->refCount = 1;
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
	header->refCount--;
	if (header->refCount > 0)
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
		header->refCount++;
	}
	return ptr;
}

static const void* RetainConstImpl(const void* ptr)
{
	if (ptr != NULL)
	{
		ManagedObjectHeader* header = ((ManagedObjectHeader*)ptr) - 1;
		header->refCount++;
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
