#pragma once

#include <string>

using byte = unsigned char;

void* locate_pattern(void* start_addr, size_t length, const byte* pattern, size_t pattern_length);