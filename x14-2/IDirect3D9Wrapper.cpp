#include "stdafx.h"
#include <d3dx9.h>
#include <d3d9.h>

#include "Font.h"
#include "IRenderable.h"
#include "IDirect3D9Wrapper.h"
#include "Direct3D9WrapperImpl.h"
#include "IDirect3DDevice9Wrapper.h"

extern Direct3D9WrapperImpl *g_pDirect3DWrapper;

HRESULT __stdcall IDirect3D9Wrapper::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
												  D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
	if (g_pDirect3DWrapper == NULL)
		return E_NOTIMPL;

	// force vertical sync
	if (g_pDirect3DWrapper->m_VSync)
		g_pDirect3DWrapper->ForceVSync(*pPresentationParameters);

	// create the Direct3D device
	HRESULT Result = CALL_D3D_VTABLE(g_pDirect3DWrapper, pfnCreateDevice)(Adapter, DeviceType, hFocusWindow, BehaviorFlags,
																		  pPresentationParameters, ppReturnedDeviceInterface);
	// notify the listener of the device creation
	g_pDirect3DWrapper->OnDeviceCreate(*ppReturnedDeviceInterface, pPresentationParameters, hFocusWindow);

	return Result;
}