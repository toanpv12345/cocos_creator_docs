#ifndef _FACEBOOK_H_
#define _FACEBOOK_H_

#include <Windows.h>
#include <string>

class IBrowserEvent
{
public:
	virtual void onLogin(std::string token) = 0;
};

typedef void(*DoPageAction)(HWND hWnd, DWORD action);

typedef long(*DisplayHTMLStr)(HWND hWnd, LPCTSTR string);

typedef long(*DisplayHTMLPage)(HWND hWnd, LPTSTR url, IBrowserEvent* browserEvent);

typedef void(*ResizeBrowser)(HWND hWnd, DWORD width, DWORD height);

typedef long(*EmbedBrowserObject)(HWND hWnd);

typedef void(*UnEmbedBrowserObject)(HWND hWnd);

typedef void(*ClearCache)();

void LoadFacebookDll();

void FreeFacebookDll();

int FacebookLogin(std::string facebookAppId, IBrowserEvent* browserEvent, int width, int height);

void ClearData();

#endif
