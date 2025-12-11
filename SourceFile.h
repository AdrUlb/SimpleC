#pragma once
#include "Util/File.h"
#include "Util/Managed.h"
#include "Util/String.h"

nullable_begin

typedef struct
{
	const char* path;
	String*nullable content;
} SourceFile;

typedef struct
{
	const SourceFile* sourceFile;
	ConstCharSpan snippet;
	size_t offset;
	size_t line;
	size_t column;
} SourceLocation;

static SourceFile* SourceFile_Init_WithPath(SourceFile* self, const char* path)
{
	self->path = path;
	self->content = File_ReadAllText(path);
	return self;
}

static void SourceFile_Fini(SourceFile* self)
{
	if (self->content)
	{
		Release(self->content);
		self->content = NULL;
	}
}

static SourceLocation SourceLocation_Create(const SourceFile* sourceFile,
                                            const size_t offset,
                                            const size_t length,
                                            const size_t line,
                                            const size_t column)
{
	const ConstCharSpan snippet = sourceFile->content
		                              ? ConstCharSpan_SubSpan(String_AsConstCharSpan((String*nonnull)sourceFile->content), offset, length)
		                              : ConstCharSpan_Empty;
	return (SourceLocation) {
		.sourceFile = sourceFile,
		.snippet = snippet,
		.offset = offset,
		.line = line,
		.column = column,
	};
}

static SourceLocation SourceLocation_Concat(const SourceLocation* first, const SourceLocation* second)
{
	assert(first->sourceFile == second->sourceFile);

	const ConstCharSpan snippet = first->sourceFile->content
		                              ? ConstCharSpan_SubSpan(
			                              String_AsConstCharSpan((String*nonnull)first->sourceFile->content),
			                              first->offset, (second->offset + second->snippet.length) - first->offset)
		                              : ConstCharSpan_Empty;

	return (SourceLocation) {
		.sourceFile = first->sourceFile,
		.snippet = snippet,
		.line = first->line,
		.column = first->column,
	};
}

nullable_end
