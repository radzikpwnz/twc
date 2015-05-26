#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _USE_MATH_DEFINES

#include <windows.h>

__declspec(dllexport) void dummy() {}

static BOOL DllInit()
{
    return TRUE;
}

static BOOL DllClose()
{
    return TRUE;
}

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) {
		return DllInit();
	} else if (fdwReason == DLL_PROCESS_DETACH) {
		return DllClose();
	}
	return TRUE;
}