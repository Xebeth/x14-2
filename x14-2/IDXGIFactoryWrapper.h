#ifndef __IDXGI_FACTORY_WRAPPER_H
#define __IDXGI_FACTORY_WRAPPER_H

class ISwapChainCreateSubscriber;

class IDXGIFactoryWrapper
{
public:
	explicit IDXGIFactoryWrapper(ISwapChainCreateSubscriber *pSubscriber_in);
	~IDXGIFactoryWrapper();

	HRESULT __stdcall EnumAdapters(UINT Adapter, IDXGIAdapter** ppAdapter);
	HRESULT __stdcall MakeWindowAssociation(HWND WindowHandle, UINT Flags);
	HRESULT __stdcall GetWindowAssociation(HWND* pWindowHandle);
	HRESULT __stdcall CreateSwapChain(IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain);
	HRESULT __stdcall CreateSoftwareAdapter(HMODULE Module, IDXGIAdapter** ppAdapter);

private:
	ISwapChainCreateSubscriber *m_pSubscriber;
};

#endif//__IDXGI_FACTORY_WRAPPER_H