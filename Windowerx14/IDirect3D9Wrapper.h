#ifndef __IDIRECT3D9_WRAPPER_H
#define __IDIRECT3D9_WRAPPER_H

class IDirect3D9Wrapper
{
public:
	HRESULT __stdcall CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType,
								   HWND hFocusWindow, DWORD BehaviorFlags,
								   D3DPRESENT_PARAMETERS* pPresentationParameters,
								   IDirect3DDevice9** ppReturnedDeviceInterface);
};

#endif//__IDIRECT3D9_WRAPPER_H