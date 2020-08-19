#include <Windows.h>
#include <cstdint>
#include <intrin.h>
#include <thread>
#include "detour64.hpp"

constexpr uintptr_t steamvr_offset = 0x21440;
constexpr uintptr_t lighthouse_offset = 0xA410A;

void* lighthouse_address = 0;

bool can_unhook = true;

Detour64 hook;
using fn_update_button_w = __int64(__fastcall*)(__int64 a1, uint32_t hand, uint32_t button, bool is_keydown, double a4);
fn_update_button_w o_update_button_w;

signed __int64 __fastcall hk_update_button_w(__int64 a1, uint32_t hand, uint32_t button, bool is_keydown, double a4) {
	can_unhook = false;
	if (_ReturnAddress() == lighthouse_address && button == 0x1) {
		can_unhook = true;
		return is_keydown ? 3 : 0;
	}
	else {
		can_unhook = true;
		return o_update_button_w(a1, hand, button, is_keydown, a4);
	}
}

void hook_function() {
	lighthouse_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(GetModuleHandle("driver_lighthouse.dll")) + lighthouse_offset);
	o_update_button_w = hook.hook_function<fn_update_button_w>(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(GetModuleHandle("vrserver.exe")) + steamvr_offset), hk_update_button_w);
}

void unhook_function() {
	while (!can_unhook);

	hook.unhook();
	Sleep(1);
	hook.cleanup();
}

BOOL WINAPI DllMain(HINSTANCE dll_module, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		hook_function();
	}
	else if (reason == DLL_PROCESS_DETACH) {
		unhook_function();
	}
	
	return TRUE;
}