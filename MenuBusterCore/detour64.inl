#pragma once
#include "detour64.hpp"

template<typename T>
T Detour64::hook_function(void* function, void* callback) {
	static_assert(std::is_pointer<T>::value, "Has to be a Pointer");

	static const uint8_t jmp_stub[] = {
		0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // jmp qword ptr [$ + 6]
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr
	};

	hook_length = calc_min_hook_length(function);

	if (hook_length < JMP_STUB_SIZE) {
		return 0;
	}

	DWORD old_protection = 0;

	if (VirtualProtect(function, hook_length, PAGE_EXECUTE_READWRITE, &old_protection) == 0) {
		return 0;
	}

	trampoline_function = allocate_trampoline(function, hook_length);

	if (!trampoline_function) {
		VirtualProtect(function, hook_length, old_protection, &old_protection);

		return 0;
	}

	hooked_function = function;

	memcpy(function, jmp_stub, JMP_STUB_SIZE);
	*reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(function) + JMP_PTR_OFFSET) = callback;

	VirtualProtect(function, hook_length, old_protection, &old_protection);

	is_hooked = true;

	return get_original<T>();
}

template<typename T>
T Detour64::get_original() {
	static_assert(std::is_pointer<T>::value, "Has to be a Pointer");

	if (is_hooked) {
		return reinterpret_cast<T>(trampoline_function);
	}
	else {
		return reinterpret_cast<T>(hooked_function);
	}
}