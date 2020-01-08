#include "BrowserEvent.h"

#include <string>
#include <vector>
#include <sstream>

STDMETHODIMP BrowserEvent::Advise(HWND hWnd, IWebBrowser2* pIE, IBrowserEvent* browserEvent)
{
	browserEvent_ = browserEvent;

	hParent_ = hWnd;
	HRESULT hr = Advise(pIE, __uuidof(DWebBrowserEvents2));
	webBrowser_ = pIE;
	webBrowser_->AddRef();

	return hr;
}

STDMETHODIMP BrowserEvent::Advise(IUnknown* pUnkCP, const IID& iid)
{
	if (pUnkCP == NULL)
		return E_INVALIDARG;

	IConnectionPointContainer* pCPC = NULL;
	HRESULT hr = pUnkCP->QueryInterface(IID_IConnectionPointContainer, reinterpret_cast<void**>(&pCPC));
	if (SUCCEEDED(hr))
	{
		IConnectionPoint* pCP = NULL;
		hr = pCPC->FindConnectionPoint(iid, &pCP);
		if (SUCCEEDED(hr))
		{
			hr = pCP->Advise(this, &dwCookie_);
			pCP->Release();
		}

		pCPC->Release();
	}

	return hr;
}

STDMETHODIMP BrowserEvent::Unadvise()
{
	HRESULT hr = Unadvise(webBrowser_, __uuidof(DWebBrowserEvents2));
	webBrowser_->Release();
	webBrowser_ = NULL;

	return hr;
}

STDMETHODIMP BrowserEvent::Unadvise(IUnknown* pUnkCP, const IID& iid)
{
	if (pUnkCP == NULL)
		return E_INVALIDARG;

	IConnectionPointContainer* pCPC = NULL;
	HRESULT hr = pUnkCP->QueryInterface(IID_IConnectionPointContainer, reinterpret_cast<void**>(&pCPC));
	if (SUCCEEDED(hr))
	{
		IConnectionPoint* pCP = NULL;
		hr = pCPC->FindConnectionPoint(iid, &pCP);
		if (SUCCEEDED(hr))
		{
			hr = pCP->Unadvise(dwCookie_);
			pCP->Release();
		}

		pCPC->Release();
	}

	return hr;
}

STDMETHODIMP BrowserEvent::Invoke(
	DISPID dispIdMember,
	REFIID riid,
	LCID lcid,
	WORD wFlags,
	DISPPARAMS *pDispParams,
	VARIANT *pVarResult,
	EXCEPINFO *pExcepInfo,
	UINT *puArgErr)
{
	switch (dispIdMember)
	{
		case DISPID_NAVIGATEERROR:
		{
			VARIANT *vt_statuscode = pDispParams->rgvarg[1].pvarVal;
			DWORD dwStatusCode = vt_statuscode->lVal;
			IDispatch *pdispFiredEvent = pDispParams->rgvarg[4].pdispVal;
			break;
		}
		case DISPID_NAVIGATECOMPLETE2:
		{
			BSTR url = ((*pDispParams).rgvarg)[0].pvarVal->bstrVal;
			std::wstring ws(url, SysStringLen(url));
			if (ws.find(L"access_token") != std::wstring::npos)
			{
				std::vector<std::wstring> pair;
				std::wstringstream ss(ws);
				std::wstring item;
				while (getline(ss, item, L'&'))
				{
					pair.push_back(item);
				}

				size_t pos = pair[0].find_first_of(L'=');
				std::wstring token = pair[0].substr(pos + 1, pair[0].size() - pos);

				if (browserEvent_)
					browserEvent_->onLogin(std::string(token.begin(), token.end()));

				SendMessage(hParent_, WM_CLOSE, 0, 0);
			}
		}
		break;
		case DISPID_BEFORENAVIGATE2:
		{
			BSTR url = ((*pDispParams).rgvarg)[5].pvarVal->bstrVal;
			std::wstring ws(url, SysStringLen(url));
			if (ws.find(L"https://www.facebook.com/reg/") != std::wstring::npos
				|| ws.find(L"https://www.facebook.com/login/identify") != std::wstring::npos)
			{
				ShellExecute(NULL, L"open", ws.c_str(), NULL, NULL, SW_SHOW);
				*(((*pDispParams).rgvarg)[0].pboolVal) = TRUE;
			}
		}
		break;
	}

	return S_OK;
}

STDMETHODIMP BrowserEvent::QueryInterface(REFIID riid, void **ppvObject)
{
	if (IsBadWritePtr(ppvObject, sizeof(void*)))
		return E_POINTER;

	(*ppvObject) = NULL;

	if (!IsEqualIID(riid, IID_IUnknown) && !IsEqualIID(riid, IID_IDispatch) && !IsEqualIID(riid, DIID_DWebBrowserEvents2))
		return E_NOINTERFACE;

	(*ppvObject) = this;
	return S_OK;
}

STDMETHODIMP_(ULONG) BrowserEvent::AddRef()
{
	return 1;
}

STDMETHODIMP_(ULONG) BrowserEvent::Release()
{
	return 1;
}

STDMETHODIMP BrowserEvent::GetTypeInfoCount(UINT *pctinfo)
{
	UNREFERENCED_PARAMETER(pctinfo);

	return E_NOTIMPL;
}

STDMETHODIMP BrowserEvent::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	UNREFERENCED_PARAMETER(iTInfo);
	UNREFERENCED_PARAMETER(lcid);
	UNREFERENCED_PARAMETER(ppTInfo);

	return E_NOTIMPL;
}

STDMETHODIMP BrowserEvent::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(rgszNames);
	UNREFERENCED_PARAMETER(cNames);
	UNREFERENCED_PARAMETER(lcid);
	UNREFERENCED_PARAMETER(rgDispId);

	return E_NOTIMPL;
}
