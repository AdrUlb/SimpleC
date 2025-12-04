#include "File.h"

#include <stdio.h>
#include "Managed.h"
#include "String.h"

FileHandle* FileHandle_Init(FileHandle* self, const char* path, const char* mode)
{
	self->file = fopen(path, mode);
	return self;
}

void FileHandle_Fini(FileHandle* handle)
{
	if (handle->file != NULL)
	{
		fclose(handle->file);
		handle->file = NULL;
	}
}

void FileHandle_SetPos(const FileHandle* handle, const size_t pos)
{
	fseek(handle->file, pos, SEEK_SET);
}

size_t FileHandle_GetPos(const FileHandle* handle)
{
	return ftell(handle->file);
}

size_t FileHandle_GetSize(const FileHandle* handle)
{
	const long currentPos = ftell(handle->file);
	fseek(handle->file, 0, SEEK_END);
	const size_t size = ftell(handle->file);
	fseek(handle->file, currentPos, SEEK_SET);
	return size;
}

void FileHandle_Read(const FileHandle* handle, void* buffer, const size_t size)
{
	fread(buffer, 1, size, handle->file);
}

void FileHandle_Write(const FileHandle* handle, const void* buffer, const size_t size)
{
	fwrite(buffer, 1, size, handle->file);
}

String* File_ReadAllText(const char* path)
{
	using const FileHandle* file = New(FileHandle, path, "r");
	const size_t size = FileHandle_GetSize(file);

	using String* str = NewWith(String, Capacity, size);
	String_Resize(str, size);
	FileHandle_Read(file, String_GetBuffer(str), size);

	return Retain(str);
}
