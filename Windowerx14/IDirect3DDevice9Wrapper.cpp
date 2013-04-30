#include "stdafx.h"
#include <d3dx9.h>
#include <d3d9.h>

#include "IRenderable.h"

#include "IDirect3D9Wrapper.h"
#include "IDirect3DDevice9Wrapper.h"
#include "Direct3DDevice9WrapperImpl.h"

extern Direct3DDevice9WrapperImpl *g_pDeviceWrapperImpl;

HRESULT __stdcall IDirect3DDevice9Wrapper::BeginScene() 
{
	if (g_pDeviceWrapperImpl == NULL)
		return E_NOTIMPL;

	HRESULT Result = S_OK;

	if (g_pDeviceWrapperImpl->m_bRender && g_pDeviceWrapperImpl->m_bSceneStarted == false)
	{
		Result = CALL_D3DDEVICE_VTABLE(g_pDeviceWrapperImpl, pfnBeginScene)();

		if (Result == D3D_OK)
			g_pDeviceWrapperImpl->Draw();
	}

	return Result;
}

HRESULT __stdcall IDirect3DDevice9Wrapper::EndScene() 
{
	if (g_pDeviceWrapperImpl == NULL)
		return E_NOTIMPL;

	HRESULT Result = S_OK;

	if (g_pDeviceWrapperImpl->m_bRender && g_pDeviceWrapperImpl->m_bSceneStarted)
	{
		Result = CALL_D3DDEVICE_VTABLE(g_pDeviceWrapperImpl, pfnEndScene)();
		g_pDeviceWrapperImpl->m_bSceneStarted = false;
	}
	
	return Result;
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	if (g_pDeviceWrapperImpl == NULL)
		return E_NOTIMPL;
		
	g_pDeviceWrapperImpl->m_bDrawUi = g_pDeviceWrapperImpl->m_bDrawUi ? true : (State == D3DRS_SCISSORTESTENABLE && Value == 0);

	return CALL_D3DDEVICE_VTABLE(g_pDeviceWrapperImpl, pfnSetRenderState)(State, Value);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) 
{
	if (g_pDeviceWrapperImpl == NULL)
		return E_NOTIMPL;

	g_pDeviceWrapperImpl->OnDeviceLost(*pPresentationParameters);

	HRESULT Result = CALL_D3DDEVICE_VTABLE(g_pDeviceWrapperImpl, pfnReset)(pPresentationParameters);

	if (Result == D3D_OK)
		g_pDeviceWrapperImpl->OnDeviceReset();
	else
		g_pDeviceWrapperImpl->m_bRender = false;

	return Result;
}