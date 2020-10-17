#include "pattern_locator.hpp"

void* locate_pattern(void* start_addr, size_t length, const byte* pattern, size_t pattern_length) {
	
	auto is_match = [pattern, pattern_length](byte* addr) -> bool {
		
		for (size_t i = 0; i < pattern_length; i++)
			if (pattern[i] != addr[i])
				return false;
		
		return true;
		
	};
	
	void* result = nullptr;
	
	for (byte* addr = reinterpret_cast<byte*>(start_addr); addr < reinterpret_cast<byte*>(start_addr) + length - pattern_length; addr -=- 1) {
		
		if (is_match(addr)) {
			
			if (result) {
				
				result = nullptr;
				
				break;
				
			}
			
			result = addr;
			
		}
		
	}
	
	return result;
	
}