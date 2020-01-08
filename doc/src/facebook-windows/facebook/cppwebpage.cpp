#include "cppwebpage.h"
#include <stdio.h>

#include "BrowserEvent.h"
#include "WebBrowserHelper.h"

BrowserEvent browserEvent;


HRESULT STDMETHODCALLTYPE MyIOleInPlaceFrame::QueryInterface(
	REFIID riid,
	__RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
{
	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE MyIOleInPlaceFrame::AddRef()
{
	return 1;
}

ULONG STDMETHODCALLTYPE MyIOleInPlaceFrame::Release()
{
	return 1;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceFrame::GetWindow(__RPC__deref_out_opt HWND *phwnd)
{
	*phwnd = ((_IOleInPlaceFrameEx *)this)->window;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceFrame::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceFrame::GetBorder(__RPC__out LPRECT lprectBorder)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceFrame::RequestBorderSpace(__RPC__in_opt LPCBORDERWIDTHS pborderwidths)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceFrame::SetBorderSpace(__RPC__in_opt LPCBORDERWIDTHS pborderwidths)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceFrame::SetActiveObject(
	__RPC__in_opt IOleInPlaceActiveObject *pActiveObject,
	__RPC__in_opt_string LPCOLESTR pszObjName)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceFrame::InsertMenus(
	__RPC__in HMENU hmenuShared,
	__RPC__inout LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceFrame::SetMenu(
	__RPC__in HMENU hmenuShared,
	__RPC__in HOLEMENU holemenu,
	__RPC__in HWND hwndActiveObject)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceFrame::RemoveMenus(__RPC__in HMENU hmenuShared)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceFrame::SetStatusText(__RPC__in_opt LPCOLESTR pszStatusText)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceFrame::EnableModeless(BOOL fEnable)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceFrame::TranslateAccelerator(__RPC__in LPMSG lpmsg, WORD wID)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyIOleClientSite::QueryInterface(
	REFIID riid,
	__RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
{
	if (!memcmp(&riid, &IID_IUnknown, sizeof(GUID)) || !memcmp(&riid, &IID_IOleClientSite, sizeof(GUID)))
		*ppvObject = &((_IOleClientSiteEx *)this)->client;
	else if (!memcmp(&riid, &IID_IOleInPlaceSite, sizeof(GUID)))
		*ppvObject = &((_IOleClientSiteEx *)this)->inplace;
	else if (!memcmp(&riid, &IID_IDocHostUIHandler, sizeof(GUID)))
		*ppvObject = &((_IOleClientSiteEx *)this)->ui;
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}

	return S_OK;
}

ULONG STDMETHODCALLTYPE MyIOleClientSite::AddRef()
{
	return 1;
}

ULONG STDMETHODCALLTYPE MyIOleClientSite::Release()
{
	return 1;
}

HRESULT STDMETHODCALLTYPE MyIOleClientSite::SaveObject()
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyIOleClientSite::GetMoniker(
	DWORD dwAssign,
	DWORD dwWhichMoniker,
	__RPC__deref_out_opt IMoniker **ppmk)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyIOleClientSite::GetContainer(__RPC__deref_out_opt IOleContainer **ppContainer)
{
	*ppContainer = 0;

	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE MyIOleClientSite::ShowObject()
{
	return NOERROR;
}

HRESULT STDMETHODCALLTYPE MyIOleClientSite::OnShowWindow(BOOL fShow)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyIOleClientSite::RequestNewObjectLayout()
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyIDocHostUIHandler::QueryInterface(
	REFIID riid,
	__RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
{
	return ((IOleClientSite *)((char *)this - sizeof(IOleClientSite) - sizeof(_IOleInPlaceSiteEx)))->QueryInterface(riid, ppvObject);
}

ULONG STDMETHODCALLTYPE MyIDocHostUIHandler::AddRef()
{
	return 1;
}

ULONG STDMETHODCALLTYPE MyIDocHostUIHandler::Release()
{
	return 1;
}

HRESULT STDMETHODCALLTYPE MyIDocHostUIHandler::ShowContextMenu(
	DWORD dwID,
	POINT *ppt,
	IUnknown *pcmdtReserved,
	IDispatch *pdispReserved)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIDocHostUIHandler::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	pInfo->cbSize = sizeof(DOCHOSTUIINFO);

	pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER;

	pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIDocHostUIHandler::ShowUI(
	DWORD dwID,
	IOleInPlaceActiveObject *pActiveObject,
	IOleCommandTarget *pCommandTarget,
	IOleInPlaceFrame *pFrame,
	IOleInPlaceUIWindow *pDoc)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIDocHostUIHandler::HideUI()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIDocHostUIHandler::UpdateUI()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIDocHostUIHandler::EnableModeless(BOOL fEnable)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIDocHostUIHandler::OnDocWindowActivate(BOOL fActivate)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIDocHostUIHandler::OnFrameWindowActivate(BOOL fActivate)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIDocHostUIHandler::ResizeBorder(
	LPCRECT prcBorder,
	IOleInPlaceUIWindow *pUIWindow,
	BOOL fRameWindow)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIDocHostUIHandler::TranslateAccelerator(
	LPMSG lpMsg,
	const GUID *pguidCmdGroup,
	DWORD nCmdID)
{
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE MyIDocHostUIHandler::GetOptionKeyPath(
	__out LPOLESTR *pchKey,
	DWORD dw)
{
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE MyIDocHostUIHandler::GetDropTarget(
	IDropTarget *pDropTarget,
	IDropTarget **ppDropTarget)
{
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE MyIDocHostUIHandler::GetExternal(IDispatch **ppDispatch)
{
	*ppDispatch = 0;
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE MyIDocHostUIHandler::TranslateUrl(
	DWORD dwTranslate,
	__in __nullterminated  OLECHAR *pchURLIn,
	__out  OLECHAR **ppchURLOut)
{
	*ppchURLOut = 0;
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE MyIDocHostUIHandler::FilterDataObject(
	IDataObject *pDO,
	IDataObject **ppDORet)
{
	*ppDORet = 0;
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceSite::QueryInterface(
	REFIID riid,
	__RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
{
	return (((MyIOleClientSite *)this) - 1)->QueryInterface(riid, ppvObject);
}

ULONG STDMETHODCALLTYPE MyIOleInPlaceSite::AddRef()
{
	return 1;
}

ULONG STDMETHODCALLTYPE MyIOleInPlaceSite::Release()
{
	return 1;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceSite::GetWindow(__RPC__deref_out_opt HWND *phwnd)
{
	*phwnd = ((_IOleInPlaceSiteEx *)this)->frame.window;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceSite::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceSite::CanInPlaceActivate()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceSite::OnInPlaceActivate()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceSite::OnUIActivate()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceSite::GetWindowContext(
	/* [out] */ __RPC__deref_out_opt IOleInPlaceFrame **ppFrame,
	/* [out] */ __RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc,
	/* [out] */ __RPC__out LPRECT lprcPosRect,
	/* [out] */ __RPC__out LPRECT lprcClipRect,
	/* [out][in] */ __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	*ppFrame = (LPOLEINPLACEFRAME)&((_IOleInPlaceSiteEx *)this)->frame;

	*ppDoc = 0;

	lpFrameInfo->fMDIApp = FALSE;
	lpFrameInfo->hwndFrame = ((_IOleInPlaceFrameEx *)*ppFrame)->window;
	lpFrameInfo->haccel = 0;
	lpFrameInfo->cAccelEntries = 0;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceSite::Scroll(SIZE scrollExtant)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceSite::OnUIDeactivate(BOOL fUndoable)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceSite::OnInPlaceDeactivate()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceSite::DiscardUndoState()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceSite::DeactivateAndUndo()
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyIOleInPlaceSite::OnPosRectChange(__RPC__in LPCRECT lprcPosRect)
{
	IOleObject *browserObject;
	IOleInPlaceObject *inplace;

	browserObject = *((IOleObject **)((char *)this - sizeof(IOleObject *) - sizeof(IOleClientSite)));
	if (!(browserObject->QueryInterface(IID_IOleInPlaceObject, (void**)&inplace)))
	{
		inplace->SetObjectRects(lprcPosRect, lprcPosRect);
	}

	return S_OK;
}

extern "C" __declspec(dllexport)
void UnEmbedBrowserObject(HWND hWnd)
{
	IOleObject **browserHandle;
	IOleObject *browserObject;

	if ((browserHandle = (IOleObject **)GetWindowLong(hWnd, GWL_USERDATA)))
	{
		browserObject = *browserHandle;

		browserObject->Close(OLECLOSE_NOSAVE);
		browserObject->Release();

		SetWindowLong(hWnd, GWL_USERDATA, 0);
	}
}

extern "C" __declspec(dllexport)
void DoPageAction(HWND hWnd, DWORD action)
{
	IWebBrowser2 *webBrowser2;
	IOleObject *browserObject;

	browserObject = *((IOleObject **)GetWindowLong(hWnd, GWL_USERDATA));

	if (!browserObject->QueryInterface(IID_IWebBrowser2, (void**)&webBrowser2))
	{
		switch (action)
		{
			case WEBPAGE_GOBACK:
				webBrowser2->GoBack();
				break;
			case WEBPAGE_GOFORWARD:
				webBrowser2->GoForward();
				break;
			case WEBPAGE_GOHOME:
				webBrowser2->GoHome();
				break;
			case WEBPAGE_SEARCH:
				webBrowser2->GoSearch();
				break;
			case WEBPAGE_REFRESH:
				webBrowser2->Refresh();
				break;
			case WEBPAGE_STOP:
				webBrowser2->Stop();
				break;
		}

		webBrowser2->Release();
	}
}

extern "C" __declspec(dllexport)
long DisplayHTMLStr(HWND hWnd, LPCTSTR string)
{
	IWebBrowser2	*webBrowser2;
	LPDISPATCH		lpDispatch;
	IHTMLDocument2	*htmlDoc2;
	IOleObject		*browserObject;
	SAFEARRAY		*sfArray;
	VARIANT			myURL;
	VARIANT			*pVar;
	BSTR			bstr;

	browserObject = *((IOleObject **)GetWindowLong(hWnd, GWL_USERDATA));

	bstr = 0;

	if (!browserObject->QueryInterface(IID_IWebBrowser2, (void**)&webBrowser2))
	{
		lpDispatch = 0;
		webBrowser2->get_Document(&lpDispatch);
		if (!lpDispatch)
		{
			VariantInit(&myURL);
			myURL.vt = VT_BSTR;
			myURL.bstrVal = SysAllocString(&Blank[0]);

			webBrowser2->Navigate2(&myURL, 0, 0, 0, 0);
			VariantClear(&myURL);
		}

		if (!webBrowser2->get_Document(&lpDispatch) && lpDispatch)
		{
			if (!lpDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&htmlDoc2))
			{
				if ((sfArray = SafeArrayCreate(VT_VARIANT, 1, (SAFEARRAYBOUND *)&ArrayBound)))
				{
					if (!SafeArrayAccessData(sfArray, (void**)&pVar))
					{
						pVar->vt = VT_BSTR;
						bstr = SysAllocString(string);

						if ((pVar->bstrVal = bstr))
						{
							htmlDoc2->write(sfArray);
							htmlDoc2->close();
						}
					}

					SafeArrayDestroy(sfArray);
				}

				htmlDoc2->Release();
			}

			lpDispatch->Release();
		}

		webBrowser2->Release();
	}

	if (bstr) 
		return 0;

	return -1;
}

extern "C" __declspec(dllexport)
long DisplayHTMLPage(HWND hWnd, LPTSTR url, IBrowserEvent* iBrowserEvent)
{
	IWebBrowser2 *webBrowser2;
	VARIANT	myURL;
	IOleObject *browserObject;

	browserObject = *((IOleObject **)GetWindowLong(hWnd, GWL_USERDATA));

	if (!browserObject->QueryInterface(IID_IWebBrowser2, (void**)&webBrowser2))
	{
		VariantInit(&myURL);
		myURL.vt = VT_BSTR;

		myURL.bstrVal = SysAllocString(url);
		if (!myURL.bstrVal)
		{
			webBrowser2->Release();
			return -6;
		}

		browserEvent.Advise(hWnd, webBrowser2, iBrowserEvent);

		webBrowser2->Navigate2(&myURL, 0, 0, 0, 0);
		VariantClear(&myURL);
		webBrowser2->Release();
		return 0;
	}

	return -5;
}

extern "C" __declspec(dllexport) 
void ResizeBrowser(HWND hWnd, DWORD width, DWORD height)
{
	IWebBrowser2 *webBrowser2;
	IOleObject *browserObject;

	browserObject = *((IOleObject **)GetWindowLong(hWnd, GWL_USERDATA));
	if (!browserObject->QueryInterface(IID_IWebBrowser2, (void**)&webBrowser2))
	{
		webBrowser2->put_Width(width);
		webBrowser2->put_Height(height);
		webBrowser2->Release();
	}
}

extern "C" __declspec(dllexport)
long EmbedBrowserObject(HWND hWnd)
{
	RECT rect;
	LPCLASSFACTORY pClassFactory;
	IOleObject *browserObject;
	IWebBrowser2 *webBrowser2;
	_IOleClientSiteEx *_iOleClientSiteEx;

	_ComIOleObjClientSiteEx *_ptr = new _ComIOleObjClientSiteEx();
	_ptr->iOleClientSiteExObj.inplace.frame.window = hWnd;
	_iOleClientSiteEx = (_IOleClientSiteEx *)((char *)_ptr + sizeof(IOleObject *));

	pClassFactory = 0;
	if (!CoGetClassObject(CLSID_WebBrowser, 3, NULL, IID_IClassFactory, (void **)&pClassFactory) && pClassFactory)
	{
		if (!pClassFactory->CreateInstance(0, IID_IOleObject, (void **)&browserObject))
		{
			pClassFactory->Release();

			*((IOleObject **)&(_ptr->iOleObj)) = browserObject;
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)(_ptr));

			if (!browserObject->SetClientSite((IOleClientSite *)_iOleClientSiteEx))
			{
				browserObject->SetHostNames(L"My Host Name", 0);

				GetClientRect(hWnd, &rect);

				if (!OleSetContainedObject((struct IUnknown *)browserObject, TRUE) &&
					!browserObject->DoVerb(OLEIVERB_SHOW, NULL, (IOleClientSite *)_iOleClientSiteEx, -1, hWnd, &rect) &&
					!browserObject->QueryInterface(IID_IWebBrowser2, (void**)&webBrowser2))
				{
					webBrowser2->put_Left(0);
					webBrowser2->put_Top(0);
					webBrowser2->put_Width(rect.right);
					webBrowser2->put_Height(rect.bottom);
					webBrowser2->Release();

					return 0;
				}
			}

			UnEmbedBrowserObject(hWnd);
			return -4;
		}

		pClassFactory->Release();
		delete _ptr;

		return -3;
	}

	delete _ptr;

	return -2;
}
