#include "stdafx.h"
#include <d3d11.h>

#include "IRenderable.h"
#include "IDXGIFactoryWrapper.h"
#include "DXGIFactoryWrapperImpl.h"
#include "ISwapChainCreateSubscriber.h"
#include <intrin.h>

extern DXGIFactoryWrapperImpl *g_pDXGIFactoryWrapper;

HRESULT __stdcall IDXGIFactoryWrapper::EnumAdapters(UINT Adapter, IDXGIAdapter** ppAdapter)
{
	if (g_pDXGIFactoryWrapper == nullptr)
		return E_NOTIMPL;

	return CALL_DXGI_FACTORY_VTABLE(g_pDXGIFactoryWrapper, pfnEnumAdapters)(Adapter, ppAdapter);
}

HRESULT __stdcall IDXGIFactoryWrapper::MakeWindowAssociation(HWND WindowHandle, UINT Flags)
{
	if (g_pDXGIFactoryWrapper == nullptr)
		return E_NOTIMPL;

	return CALL_DXGI_FACTORY_VTABLE(g_pDXGIFactoryWrapper, pfnMakeWindowAssociation)(WindowHandle, Flags);
}

HRESULT __stdcall IDXGIFactoryWrapper::GetWindowAssociation(HWND* pWindowHandle)
{
	if (g_pDXGIFactoryWrapper == nullptr)
		return E_NOTIMPL;
	
	return CALL_DXGI_FACTORY_VTABLE(g_pDXGIFactoryWrapper, pfnGetWindowAssociation)(pWindowHandle);
}

HRESULT __stdcall IDXGIFactoryWrapper::CreateSwapChain(IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain)
{
	if (g_pDXGIFactoryWrapper == nullptr)
		return E_NOTIMPL;
			
	// create the Direct3D device
	HRESULT result = CALL_DXGI_FACTORY_VTABLE(g_pDXGIFactoryWrapper, pfnCreateSwapChain)(pDevice, pDesc, ppSwapChain);

	if (ppSwapChain != nullptr)
		g_pDXGIFactoryWrapper->OnSwapChainCreate(*ppSwapChain, pDesc);

	return result;
}

HRESULT __stdcall IDXGIFactoryWrapper::CreateSoftwareAdapter(HMODULE Module, IDXGIAdapter** ppAdapter)
{
	if (g_pDXGIFactoryWrapper == nullptr)
		return E_NOTIMPL;

	return CALL_DXGI_FACTORY_VTABLE(g_pDXGIFactoryWrapper, pfnCreateSoftwareAdapter)(Module, ppAdapter);
}
