#pragma once
#include "Util/File.h"
#include "Util/Macros.h"
#include "Util/Managed.h"
#include "Util/String.h"

typedef struct
{
	const char* path;
	String* content;
} SourceFile;

typedef struct
{
	const SourceFile* sourceFile;
	ConstCharSpan snippet;
	size_t offset;
	size_t line;
	size_t column;
} SourceLocation;

always_inline SourceFile* SourceFile_Init_WithPath(SourceFile* self, const char* path)
{
	self->path = path;
	self->content = File_ReadAllText(path);
	if (self->content == NULL)
	{
		Release(self);
		return NULL;
	}
	return self;
}

always_inline void SourceFile_Fini(SourceFile* self)
{
	if (self->content)
	{
		Release(self->content);
		self->content = NULL;
	}
}

always_inline SourceLocation SourceLocation_Create(const SourceFile* sourceFile,
                                                   const size_t offset,
                                                   const size_t length,
                                                   const size_t line,
                                                   const size_t column)
{
	return (SourceLocation) {
		.sourceFile = sourceFile,
		.snippet = ConstCharSpan_SubSpan(String_AsConstCharSpan(sourceFile->content), offset, length),
		.offset = offset,
		.line = line,
		.column = column,
	};
}

always_inline SourceLocation SourceLocation_Concat(const SourceLocation* first, const SourceLocation* second)
{
	assert(first->sourceFile == second->sourceFile);

	return (SourceLocation) {
		.sourceFile = first->sourceFile,
		.snippet = ConstCharSpan_SubSpan(
			String_AsConstCharSpan(first->sourceFile->content), first->offset,
			(second->offset + second->snippet.length) - first->offset),
		.line = first->line,
		.column = first->column,
	};
}
