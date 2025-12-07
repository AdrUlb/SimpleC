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
		AstExpression.h
		AstTypeName.h
		CompilerError.h
		Lexer.c
		Lexer.h
		Parser.c
		Parser.h
		SourceFile.h
		Token.c
		Token.h
)
add_executable(SimpleC ${UTIL_SOURCES} ${SOURCES})

target_compile_options(SimpleC PRIVATE -Wall -Wextra -Wno-unused -pedantic)
