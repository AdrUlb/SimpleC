cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_FLAGS_DEBUG_INIT "-O0 -g -fsanitize=address")
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3")

project(SimpleC C)

set(CMAKE_C_STANDARD 11)

set(UTIL_SOURCES
		Util/Array.h
		Util/ArrayDef.h
		Util/File.c
		Util/File.h
		Util/List.h
		Util/ListDef.h
		Util/Managed.h
		Util/Span.h
		Util/String.c
		Util/String.h
)

set(SOURCES
		main.c
		Lexer.c
		Parser.c
		Token.c
)
add_executable(SimpleC ${UTIL_SOURCES} ${SOURCES})

target_compile_options(SimpleC PRIVATE -Wall -Wextra -Wno-unused -pedantic)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
	target_compile_options(SimpleC PRIVATE
			-Weverything
			-Wno-nullability-extension
			-Wno-declaration-after-statement
			-Wno-unsafe-buffer-usage
			-Wno-padded
			-Wno-pre-c11-compat
			-Wno-covered-switch-default
			-Wno-cast-function-type-strict
			-Wno-incompatible-function-pointer-types-strict
			-Wno-language-extension-token
			-Wno-switch-enum
			-Werror=nullability
			-Werror=null-dereference
			-Werror=nullability-completeness
			-Werror=nullable-to-nonnull-conversion
			-Werror=nonnull)
endif ()
