#include "stdafx.h"
#include <d3d11.h>

#include "IRenderable.h"
#include "IDXGISwapChainWrapper.h"
#include "DXGISwapChainWrapperImpl.h"
#include "IDeviceCreateSubscriber.h"


extern DXGISwapChainWrapperImpl *g_pDXGISwapChainWrapper;

HRESULT __stdcall IDXGISwapChainWrapper::Present(UINT SyncInterval, UINT Flags)
{
	if (g_pDXGISwapChainWrapper == nullptr)
		return E_NOTIMPL;
	
	if (g_pDXGISwapChainWrapper->IsRendering())
	{
		g_pDXGISwapChainWrapper->Draw();

		// create the Direct3D device
		return CALL_DXGI_SWAPCHAIN_VTABLE(g_pDXGISwapChainWrapper, pfnPresent)(SyncInterval, Flags);
	}

	return S_OK;
}