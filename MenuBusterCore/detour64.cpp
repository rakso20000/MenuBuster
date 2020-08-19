#include "detour64.hpp"

bool Detour64::unhook() {
	if (!is_hooked) {
		return false;
	}

	DWORD old_protection = 0;

	if (VirtualProtect(hooked_function, hook_length, PAGE_EXECUTE_READWRITE, &old_protection) == 0) {
		return 0;
	}

	memcpy(hooked_function, trampoline_function, hook_length);

	VirtualProtect(hooked_function, hook_length, old_protection, &old_protection);

	is_hooked = false;

	return true;
}

void Detour64::cleanup() {
	if (trampoline_function) {
		VirtualFree(trampoline_function, 0, MEM_RELEASE);
	}
}

void* Detour64::allocate_trampoline(void* function, size_t length) {
	static const uint8_t jmp_stub[] = {
		0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // jmp qword ptr [$ + 6]
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr
	};

	void* tramp = VirtualAlloc(0, length + JMP_STUB_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (!tramp) {
		return 0;
	}

	memcpy(tramp, function, length);
	memcpy(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(tramp) + length), jmp_stub, JMP_STUB_SIZE);
	*reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(tramp) + length + JMP_PTR_OFFSET) = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(function) + length);

	return tramp;
}

size_t Detour64::calc_min_hook_length(void* function) {
	ldasm_data ld = { 0 };

	size_t length = 0;

	while (length < JMP_STUB_SIZE) {
		length += ldasm(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(function) + length), &ld, 0);
	}

	return length;
}