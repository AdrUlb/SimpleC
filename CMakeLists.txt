cmake_minimum_required(VERSION 4.1)

set(CMAKE_C_FLAGS_DEBUG_INIT "-O0 -g -fsanitize=address")
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3")

project(SimpleC C)

set(CMAKE_C_STANDARD 11)

set(SOURCES
		Util/New.h
		Util/SpanDef.h
		Util/Span.h
		Util/ArrayDef.h
		Util/Array.h
		Util/ListDef.h
		Util/List.h
		Util/String.h
		Util/String.c
		Util/File.h
		Util/File.c
		Lexer.h
		Lexer.c
		Token.h
		Token.c
		Parser.h
		Parser.c
		main.c)
add_executable(SimpleC ${SOURCES})

target_compile_options(SimpleC PRIVATE -Wall -Wextra -Wno-unused)
