#include "stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>

#include "Font.h"
#include "IRenderable.h"
#include "IDirect3DDevice9Wrapper.h"
#include "Direct3DDevice9WrapperImpl.h"
#include "IDirect3DSwapChain9Wrapper.h"
#include "Direct3DSwapChain9WrapperImpl.h"

extern Direct3DSwapChain9WrapperImpl *g_pDirect3DSwapChainWrapper;
extern Direct3DDevice9WrapperImpl *g_pDeviceWrapperImpl;

HRESULT __stdcall IDirect3DSwapChain9Wrapper::Present(CONST RECT* pSourceRect,CONST RECT* pDestRect, HWND hDestWindowOverride,
													  CONST RGNDATA* pDirtyRegion, DWORD dwFlags)
{
	if (g_pDirect3DSwapChainWrapper == NULL)
		return E_NOTIMPL;
	
	if (g_pDeviceWrapperImpl->IsRendering())
	{
		g_pDeviceWrapperImpl->Draw();
		g_pDeviceWrapperImpl->SetHooks();

		// create the Direct3D device
		return CALL_D3DSWAPCHAIN_VTABLE(g_pDirect3DSwapChainWrapper, pfnPresent)(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
	}

	return D3D_OK;
}