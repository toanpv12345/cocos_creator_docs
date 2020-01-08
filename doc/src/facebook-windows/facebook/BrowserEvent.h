#pragma once

#include <MsHTML.h>
#include <Exdisp.h>
#include <ExDispid.h>

#include <string>

class IBrowserEvent
{
public:
	virtual void onLogin(std::string token) = 0;
};

class BrowserEvent: public IDispatch
{
public:
	// No constructor or destructor is needed
	// IUnknown methods
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	
	// IDispatch methods
	STDMETHODIMP GetTypeInfoCount(UINT *pctinfo);
	STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
	STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

	STDMETHODIMP Advise(HWND hWnd, IWebBrowser2* pIE, IBrowserEvent* browserEvent);
	STDMETHODIMP Unadvise();
private:
	STDMETHODIMP Advise(IUnknown* pUnkCP, const IID& iid);
	STDMETHODIMP Unadvise(IUnknown* pUnkCP, const IID& iid);
private:
	IBrowserEvent* browserEvent_;
	IWebBrowser2* webBrowser_;
	DWORD dwCookie_;
	HWND hParent_;
};

