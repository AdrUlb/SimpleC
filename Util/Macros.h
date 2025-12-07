#pragma once

#define STRINGIFY(str) #str
#define CONCAT(a, b) a##b
#define EXPAND_AND_CONCAT(a, b) CONCAT(a, b)
