#pragma once

#include "String.h"

nullable_begin

typedef struct
{
	FILE*nullable file;
} FileHandle;

FileHandle* FileHandle_Init_WithArgs(FileHandle* self, const char* path, const char* mode);
void FileHandle_Fini(FileHandle* handle);
void FileHandle_SetPos(const FileHandle* handle, size_t pos);
size_t FileHandle_GetPos(const FileHandle* handle);
size_t FileHandle_GetSize(const FileHandle* handle);
void FileHandle_Read(const FileHandle* handle, void* buffer, size_t size);
void FileHandle_Write(const FileHandle* handle, const void* buffer, size_t size);
String*nullable File_ReadAllText(const char* path);

nullable_end
