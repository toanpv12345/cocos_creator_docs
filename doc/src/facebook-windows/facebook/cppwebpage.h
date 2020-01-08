#ifndef CPPWEBPAGE_H
#define CPPWEBPAGE_H

#include <windows.h>
#include <exdisp.h>		// Defines of stuff like IWebBrowser2. This is an include file with Visual C 6 and above
#include <mshtml.h>		// Defines of stuff like IHTMLDocument2. This is an include file with Visual C 6 and above
#include <mshtmhst.h>	// Defines of stuff like IDocHostUIHandler. This is an include file with Visual C 6 and above
#include <crtdbg.h>		// for _ASSERT()
#include <tchar.h>		// for _tcsnicmp which compares two UNICODE strings case-insensitive

#include "BrowserEvent.h"

static wchar_t Blank[] = { L"about:blank" };
static const SAFEARRAYBOUND ArrayBound = { 1, 0 };

class MyIOleInPlaceFrame: public IOleInPlaceFrame
{
public:
	HRESULT STDMETHODCALLTYPE QueryInterface(
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG STDMETHODCALLTYPE AddRef();

	ULONG STDMETHODCALLTYPE Release();

public: /* IOleWindow */
	/* [input_sync] */ HRESULT STDMETHODCALLTYPE GetWindow(
		/* [out] */ __RPC__deref_out_opt HWND *phwnd);

	HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(
		/* [in] */ BOOL fEnterMode);

public: /* IDispatch */
	/* [input_sync] */ HRESULT STDMETHODCALLTYPE GetBorder(
		/* [out] */ __RPC__out LPRECT lprectBorder);

	/* [input_sync] */ HRESULT STDMETHODCALLTYPE RequestBorderSpace(
		/* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths);

	/* [input_sync] */ HRESULT STDMETHODCALLTYPE SetBorderSpace(
		/* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths);

	HRESULT STDMETHODCALLTYPE SetActiveObject(
		/* [unique][in] */ __RPC__in_opt IOleInPlaceActiveObject *pActiveObject,
		/* [unique][string][in] */ __RPC__in_opt_string LPCOLESTR pszObjName);


public: /* IOleInPlaceFrame */
	HRESULT STDMETHODCALLTYPE InsertMenus(
		/* [in] */ __RPC__in HMENU hmenuShared,
		/* [out][in] */ __RPC__inout LPOLEMENUGROUPWIDTHS lpMenuWidths);

	/* [input_sync] */ HRESULT STDMETHODCALLTYPE SetMenu(
		/* [in] */ __RPC__in HMENU hmenuShared,
		/* [in] */ __RPC__in HOLEMENU holemenu,
		/* [in] */ __RPC__in HWND hwndActiveObject);

	HRESULT STDMETHODCALLTYPE RemoveMenus(
		/* [in] */ __RPC__in HMENU hmenuShared);

	/* [input_sync] */ HRESULT STDMETHODCALLTYPE SetStatusText(
		/* [unique][in] */ __RPC__in_opt LPCOLESTR pszStatusText);

	HRESULT STDMETHODCALLTYPE EnableModeless(
		/* [in] */ BOOL fEnable);

	HRESULT STDMETHODCALLTYPE TranslateAccelerator(
		/* [in] */ __RPC__in LPMSG lpmsg,
		/* [in] */ WORD wID);
};

class MyIOleClientSite: public IOleClientSite
{
public:
	HRESULT STDMETHODCALLTYPE QueryInterface(
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG STDMETHODCALLTYPE AddRef();

	ULONG STDMETHODCALLTYPE Release();

public:
	HRESULT STDMETHODCALLTYPE SaveObject();

	HRESULT STDMETHODCALLTYPE GetMoniker(
		/* [in] */ DWORD dwAssign,
		/* [in] */ DWORD dwWhichMoniker,
		/* [out] */ __RPC__deref_out_opt IMoniker **ppmk);

	HRESULT STDMETHODCALLTYPE GetContainer(
		/* [out] */ __RPC__deref_out_opt IOleContainer **ppContainer);

	HRESULT STDMETHODCALLTYPE ShowObject();

	HRESULT STDMETHODCALLTYPE OnShowWindow(
		/* [in] */ BOOL fShow);

	HRESULT STDMETHODCALLTYPE RequestNewObjectLayout();
};

class MyIDocHostUIHandler: public IDocHostUIHandler
{
public:
	HRESULT STDMETHODCALLTYPE QueryInterface(
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG STDMETHODCALLTYPE AddRef();

	ULONG STDMETHODCALLTYPE Release();

public:
	HRESULT STDMETHODCALLTYPE ShowContextMenu(
		/* [in] */ DWORD dwID,
		/* [in] */ POINT *ppt,
		/* [in] */ IUnknown *pcmdtReserved,
		/* [in] */ IDispatch *pdispReserved);

	HRESULT STDMETHODCALLTYPE GetHostInfo(
		/* [out][in] */ DOCHOSTUIINFO *pInfo);

	HRESULT STDMETHODCALLTYPE ShowUI(
		/* [in] */ DWORD dwID,
		/* [in] */ IOleInPlaceActiveObject *pActiveObject,
		/* [in] */ IOleCommandTarget *pCommandTarget,
		/* [in] */ IOleInPlaceFrame *pFrame,
		/* [in] */ IOleInPlaceUIWindow *pDoc);

	HRESULT STDMETHODCALLTYPE HideUI();

	HRESULT STDMETHODCALLTYPE UpdateUI();

	HRESULT STDMETHODCALLTYPE EnableModeless(
		/* [in] */ BOOL fEnable);

	HRESULT STDMETHODCALLTYPE OnDocWindowActivate(
		/* [in] */ BOOL fActivate);

	HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(
		/* [in] */ BOOL fActivate);

	HRESULT STDMETHODCALLTYPE ResizeBorder(
		/* [in] */ LPCRECT prcBorder,
		/* [in] */ IOleInPlaceUIWindow *pUIWindow,
		/* [in] */ BOOL fRameWindow);

	HRESULT STDMETHODCALLTYPE TranslateAccelerator(
		/* [in] */ LPMSG lpMsg,
		/* [in] */ const GUID *pguidCmdGroup,
		/* [in] */ DWORD nCmdID);

	HRESULT STDMETHODCALLTYPE GetOptionKeyPath(
		/* [annotation][out] */
		__out  LPOLESTR *pchKey,
		/* [in] */ DWORD dw);

	HRESULT STDMETHODCALLTYPE GetDropTarget(
		/* [in] */ IDropTarget *pDropTarget,
		/* [out] */ IDropTarget **ppDropTarget);

	HRESULT STDMETHODCALLTYPE GetExternal(
		/* [out] */ IDispatch **ppDispatch);

	HRESULT STDMETHODCALLTYPE TranslateUrl(
		/* [in] */ DWORD dwTranslate,
		/* [annotation][in] */
		__in __nullterminated  OLECHAR *pchURLIn,
		/* [annotation][out] */
		__out  OLECHAR **ppchURLOut);

	HRESULT STDMETHODCALLTYPE FilterDataObject(
		/* [in] */ IDataObject *pDO,
		/* [out] */ IDataObject **ppDORet);
};

class MyIOleInPlaceSite : public IOleInPlaceSite
{
public:
	HRESULT STDMETHODCALLTYPE QueryInterface(
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG STDMETHODCALLTYPE AddRef();

	ULONG STDMETHODCALLTYPE Release();

public: /* IOleWindow */
	/* [input_sync] */ HRESULT STDMETHODCALLTYPE GetWindow(
		/* [out] */ __RPC__deref_out_opt HWND *phwnd);

	HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(
		/* [in] */ BOOL fEnterMode);

public:
	HRESULT STDMETHODCALLTYPE CanInPlaceActivate();

	HRESULT STDMETHODCALLTYPE OnInPlaceActivate();

	HRESULT STDMETHODCALLTYPE OnUIActivate();

	HRESULT STDMETHODCALLTYPE GetWindowContext(
		/* [out] */ __RPC__deref_out_opt IOleInPlaceFrame **ppFrame,
		/* [out] */ __RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc,
		/* [out] */ __RPC__out LPRECT lprcPosRect,
		/* [out] */ __RPC__out LPRECT lprcClipRect,
		/* [out][in] */ __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo);

	HRESULT STDMETHODCALLTYPE Scroll(
		/* [in] */ SIZE scrollExtant);

	HRESULT STDMETHODCALLTYPE OnUIDeactivate(
		/* [in] */ BOOL fUndoable);

	HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate();

	HRESULT STDMETHODCALLTYPE DiscardUndoState();

	HRESULT STDMETHODCALLTYPE DeactivateAndUndo();

	HRESULT STDMETHODCALLTYPE OnPosRectChange(
		/* [in] */ __RPC__in LPCRECT lprcPosRect);
};

typedef struct 
{
	MyIOleInPlaceFrame	frame;		// The IOleInPlaceFrame must be first!
	HWND				window;
} _IOleInPlaceFrameEx;

typedef struct 
{
	MyIOleInPlaceSite		inplace;	// My IOleInPlaceSite object. Must be first with in _IOleInPlaceSiteEx.
	_IOleInPlaceFrameEx		frame;		// My IOleInPlaceFrame object. Must be first within my _IOleInPlaceFrameEx
} _IOleInPlaceSiteEx;

typedef struct 
{
	MyIDocHostUIHandler		ui;			// My IDocHostUIHandler object. Must be first.
} _IDocHostUIHandlerEx;

typedef struct 
{
	MyIOleClientSite		client;		// My IOleClientSite object. Must be first.
	_IOleInPlaceSiteEx		inplace;	// My IOleInPlaceSite object. A convenient place to put it.
	_IDocHostUIHandlerEx	ui;			// My IDocHostUIHandler object. Must be first within my _IDocHostUIHandlerEx.
} _IOleClientSiteEx;

typedef struct 
{
	IOleObject				*iOleObj;
	_IOleClientSiteEx		iOleClientSiteExObj;
} _ComIOleObjClientSiteEx;

#define WEBPAGE_GOBACK		0
#define WEBPAGE_GOFORWARD	1
#define WEBPAGE_GOHOME		2
#define WEBPAGE_SEARCH		3
#define WEBPAGE_REFRESH		4
#define WEBPAGE_STOP		5

/*
void DoPageAction(HWND hWnd, DWORD action);

long DisplayHTMLStr(HWND hWnd, LPCTSTR string);

long DisplayHTMLPage(HWND hWnd, LPTSTR url, IBrowserEvent* browserEvent);

void ResizeBrowser(HWND hWnd, DWORD width, DWORD height);

long EmbedBrowserObject(HWND hWnd);

void UnEmbedBrowserObject(HWND hWnd);
*/

#endif // CPPWEBPAGE_H