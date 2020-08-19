#pragma once
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <windows.h>

#include "ldasm.h"

class Detour64 {
public:

	void* hooked_function = 0;
	void* trampoline_function = 0;
	size_t hook_length = 0;

	bool is_in_thread = false;
	bool is_hooked = false;

	static const size_t JMP_STUB_SIZE = 14;
	static const size_t JMP_PTR_OFFSET = 6;

public:

	template<typename T>
	T hook_function(void* function, void* callback);
	template<typename T>
	T get_original();
	bool unhook();
	void cleanup();

private:

	void* allocate_trampoline(void* function, size_t length);
	size_t calc_min_hook_length(void* function);

};

#include "detour64.inl"