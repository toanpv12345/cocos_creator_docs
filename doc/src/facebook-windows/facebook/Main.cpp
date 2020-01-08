#include <Windows.h>
#include "WebBrowserHelper.h"

extern "C" __declspec(dllexport)
void ClearCache()
{
	Utilities::WebBrowser::WebBrowserHelper::ClearCache();
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}
