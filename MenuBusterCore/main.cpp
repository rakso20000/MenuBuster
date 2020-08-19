#include <Windows.h>

void on_load(HMODULE mod) {
	FreeLibraryAndExitThread(mod, 0);
}

BOOL WINAPI DllMain(HINSTANCE dll_module, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		HANDLE thread_handle = CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(on_load), reinterpret_cast<void*>(dll_module), 0, 0);
		
		if (thread_handle) {
			CloseHandle(thread_handle);
		}
	}
	else if (reason == DLL_PROCESS_DETACH) {
		
	}
	
	return true;
}