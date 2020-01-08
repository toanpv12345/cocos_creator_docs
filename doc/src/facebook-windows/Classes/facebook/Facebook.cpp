#include "Facebook.h"

#include <windows.h>

unsigned char WindowCount = 0;
static const TCHAR ClassName[] = L"Facebook";

#define WINDOW_WIDTH	800
#define WINDOW_HEIGHT	600

int scrWidth = WINDOW_WIDTH;
int scrHeight = WINDOW_HEIGHT;

HMODULE hModule = NULL;
DoPageAction pfnDoPageAction;
DisplayHTMLStr pfnDisplayHTMLStr;
DisplayHTMLPage pfnDisplayHTMLPage;
ResizeBrowser pfnResizeBrowser;
EmbedBrowserObject pfnEmbedBrowserObject;
UnEmbedBrowserObject pfnUnEmbedBrowserObject;
ClearCache pfnClearCache;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_SIZE:
			pfnResizeBrowser(hWnd, LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_CREATE:
		{
			if (pfnEmbedBrowserObject(hWnd))
				return -1;

			++WindowCount;

			int screenWidth = GetSystemMetrics(SM_CXSCREEN);
			int screenHeight = GetSystemMetrics(SM_CYSCREEN);

			MoveWindow(
				hWnd,
				(screenWidth - scrWidth) / 2, (screenHeight - scrHeight) / 2,
				scrWidth, scrHeight,
				TRUE
			);
			return 0;
		}
		case WM_DESTROY:
		{
			(*pfnUnEmbedBrowserObject)(hWnd);

			--WindowCount;

			if (!WindowCount)
				PostQuitMessage(0);

			return TRUE;
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void LoadFacebookDll()
{
	if (hModule == NULL)
	{
		hModule = LoadLibrary(L"facebook.dll");
		pfnDoPageAction = (DoPageAction)GetProcAddress(hModule, "DoPageAction");
		pfnDisplayHTMLStr = (DisplayHTMLStr)GetProcAddress(hModule, "DisplayHTMLStr");
		pfnDisplayHTMLPage = (DisplayHTMLPage)GetProcAddress(hModule, "DisplayHTMLPage");
		pfnResizeBrowser = (ResizeBrowser)GetProcAddress(hModule, "ResizeBrowser");
		pfnEmbedBrowserObject = (EmbedBrowserObject)GetProcAddress(hModule, "EmbedBrowserObject");
		pfnUnEmbedBrowserObject = (UnEmbedBrowserObject)GetProcAddress(hModule, "UnEmbedBrowserObject");
		pfnClearCache = (ClearCache)GetProcAddress(hModule, "ClearCache");
	}
}

void FreeFacebookDll()
{
	if (hModule)
	{
		FreeLibrary(hModule);
		hModule = NULL;
	}
}

int FacebookLogin(std::string facebookAppId, IBrowserEvent* browserEvent, int width, int height)
{
	MSG	msg;

	scrWidth = width;
	scrHeight = height;

	std::wstring url = L"https://www.facebook.com/v2.9/dialog/oauth?";
	url += L"client_id=" + std::wstring(facebookAppId.begin(), facebookAppId.end());
	url += L"&display=popup&response_type=token&redirect_uri=https://www.facebook.com/connect/login_success.html";

	if (OleInitialize(NULL) == S_OK)
	{
		WNDCLASSEX wc;

		ZeroMemory(&wc, sizeof(WNDCLASSEX));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpfnWndProc = WindowProc;
		wc.lpszClassName = &ClassName[0];
		RegisterClassEx(&wc);

		msg.hwnd = CreateWindowEx(
			WS_EX_TOPMOST, &ClassName[0], L"Facebook",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
			HWND_DESKTOP, NULL, wc.hInstance, 0
		);

		if (msg.hwnd)
		{
			(*pfnDisplayHTMLPage)(msg.hwnd, (WCHAR*)url.c_str(), browserEvent);

			ShowWindow(msg.hwnd, SW_SHOWNORMAL);
			UpdateWindow(msg.hwnd);
		}

		while (GetMessage(&msg, 0, 0, 0) == 1)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		OleUninitialize();

		return 0;
	}

	return -1;
}

void ClearData()
{
	(*pfnClearCache)();
}