#include "stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>

#include "IRenderable.h"

#include "IDirect3D9Wrapper.h"
#include "IDirect3DDevice9Wrapper.h"
#include "Direct3DDevice9WrapperImpl.h"
#include "Direct3DSwapChain9WrapperImpl.h"

extern Direct3DSwapChain9WrapperImpl *g_pDirect3DSwapChainWrapper;
extern Direct3DDevice9WrapperImpl *g_pDeviceWrapperImpl;

HRESULT __stdcall IDirect3DDevice9Wrapper::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) 
{
	if (g_pDeviceWrapperImpl == NULL)
		return E_NOTIMPL;

	g_pDeviceWrapperImpl->OnDeviceLost(*pPresentationParameters);

	if (g_pDirect3DSwapChainWrapper != NULL)
	{
		g_pDirect3DSwapChainWrapper->OnDeviceLost();
		// cleanup
		delete g_pDirect3DSwapChainWrapper;
		g_pDirect3DSwapChainWrapper = NULL;
	}

	HRESULT Result = CALL_D3DDEVICE_VTABLE(g_pDeviceWrapperImpl, pfnReset)(pPresentationParameters);

	if (Result == D3D_OK)
		g_pDeviceWrapperImpl->OnDeviceReset();

	return Result;
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9** pSwapChain)
{
	if (g_pDeviceWrapperImpl == NULL)
		return E_NOTIMPL;

	HRESULT Result = CALL_D3DDEVICE_VTABLE(g_pDeviceWrapperImpl, pfnCreateAdditionalSwapChain)(pPresentationParameters, pSwapChain);

	if (Result == D3D_OK && *pSwapChain != NULL && g_pDirect3DSwapChainWrapper == NULL && g_pDeviceWrapperImpl->IsFullscreen() == false)
	{
		g_pDirect3DSwapChainWrapper = new Direct3DSwapChain9WrapperImpl(*pSwapChain);
	}

	return Result;
}

HRESULT __stdcall IDirect3DDevice9Wrapper::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
	if (g_pDeviceWrapperImpl == NULL)
		return E_NOTIMPL;
	
	if (g_pDeviceWrapperImpl->IsRendering())
	{
		g_pDeviceWrapperImpl->Draw();
		g_pDeviceWrapperImpl->SetHooks();

		return CALL_D3DDEVICE_VTABLE(g_pDeviceWrapperImpl, pfnPresent)(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	}

	return D3D_OK;
}