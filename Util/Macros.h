#pragma once

#define STRINGIFY(str) #str
#define CONCAT(a, b) a##b
#define EXPAND_AND_CONCAT(a, b) CONCAT(a, b)

#if defined(__clang__) && !defined(__JETBRAINS_IDE__)
#define nullable _Nullable
#define nonnull _Nonnull
#define nullable_begin _Pragma("clang assume_nonnull begin")
#define nullable_end _Pragma("clang assume_nonnull end")
#else
#define nullable
#define nonnull
#define nullable_begin
#define nullable_end
#endif
