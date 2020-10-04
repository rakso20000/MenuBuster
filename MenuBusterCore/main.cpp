#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <cstdint>
#include <intrin.h>

#include "detour64.hpp"

constexpr uintptr_t steamvr_offset = 0x22180;
constexpr uintptr_t lighthouse_offset = 0xA424A;
constexpr uint64_t steamvr_sign = 0xC8B60F4538EC8348;

void* lighthouse_address = 0;

bool disable_menu = false;
bool menu_down = false;
bool can_unhook = true;

union Button {
	uint64_t raw;
	struct {
		uint32_t button;
		uint32_t hand;
	};
};

Detour64 hook;
using fn_update_button_w = __int64(__fastcall*)(__int64 a1, Button button, bool is_keydown, double a4);
fn_update_button_w o_update_button_w;

struct IpcMessage {
	
	bool in_song;
	bool should_exit;
	
};

signed __int64 __fastcall hk_update_button_w(__int64 a1, Button button, bool is_keydown, double a4) {
	can_unhook = false;
	
	if (_ReturnAddress() == lighthouse_address && button.button == 0x1) {
		__int64 ret;
		if ((disable_menu && !menu_down) || (!is_keydown && !menu_down)) {
			ret = is_keydown ? 3 : 0;
		}
		else {
			ret = o_update_button_w(a1, button, is_keydown, a4);
			menu_down = is_keydown;
		}
		
		can_unhook = true;
		return ret;
	}
	else {
		can_unhook = true;
		return o_update_button_w(a1, button, is_keydown, a4);
	}
}

void unload(HMODULE module, SOCKET sock) {
	if (sock != INVALID_SOCKET) {
		shutdown(sock, SD_BOTH);
		closesocket(sock);
	}
	
	WSACleanup();
	
	FreeLibraryAndExitThread(module, 0);
}

void thread(HMODULE module) {
	
	WSADATA wsaData;
	
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (err != 0) {
		unload(module, INVALID_SOCKET);
	}
	
	addrinfo* result = NULL,
		* ptr = NULL,
		hints;
	
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	
	err = getaddrinfo("127.0.0.1", "12616", &hints, &result);
	
	if (err != 0) {
		unload(module, INVALID_SOCKET);
		return;
	}
	
	ptr = result;
	
	SOCKET sock = INVALID_SOCKET;
	sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	
	if (sock == INVALID_SOCKET) {
		freeaddrinfo(result);
		unload(module, sock);
		return;
	}
	
	err = connect(sock, ptr->ai_addr, ptr->ai_addrlen);
	if (err == SOCKET_ERROR) {
		freeaddrinfo(result);
		unload(module, sock);
		return;
	}
	
	freeaddrinfo(result);
	
	IpcMessage msg;
	
	int bytes_read = 0;
	
	while (bytes_read != SOCKET_ERROR) {
		
		bytes_read = recv(sock, reinterpret_cast<char*>(&msg), sizeof(IpcMessage), MSG_WAITALL);
		
		if (msg.should_exit)
			break;
		
		disable_menu = msg.in_song;
		
	}
	
	unload(module, sock);
	
}

void init_thread(HMODULE module) {
	HANDLE thread_handle = CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(thread), reinterpret_cast<void*>(module), 0, 0);
	
	if (thread_handle != INVALID_HANDLE_VALUE) {
		CloseHandle(thread_handle);
	}
}

void display_error(uint64_t* update_button_w) {
	MessageBox(0, "MenuBuster was developed for a different version of SteamVR", "MenuBusterCore Error", 0);
}

void hook_function() {
	lighthouse_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(GetModuleHandle("driver_lighthouse.dll")) + lighthouse_offset);
	
	uint64_t* update_button_w = reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(GetModuleHandle("vrserver.exe")) + steamvr_offset);
	
	if (*update_button_w != steamvr_sign) {
		HANDLE thread_handle = CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(display_error), update_button_w, 0, 0);
		
		if (thread_handle != INVALID_HANDLE_VALUE) {
			CloseHandle(thread_handle);
		}
		
		return;
	}
	
	o_update_button_w = hook.hook_function<fn_update_button_w>(update_button_w, hk_update_button_w);
}

void unhook_function() {
	while (!can_unhook);

	hook.unhook();
	Sleep(1);
	hook.cleanup();
}

BOOL WINAPI DllMain(HINSTANCE dll_module, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		init_thread(dll_module);
		hook_function();
	}
	else if (reason == DLL_PROCESS_DETACH) {
		unhook_function();
	}
	
	return TRUE;
}