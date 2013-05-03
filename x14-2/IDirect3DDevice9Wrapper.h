#ifndef __IDIRECT3DDEVICE9_WRAPPER_H
#define __IDIRECT3DDEVICE9_WRAPPER_H

class IDirect3DDevice9Wrapper
{
public:
	HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* pPresentationParameters);
	HRESULT __stdcall BeginScene();
	HRESULT __stdcall EndScene();
	HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE State,DWORD Value);
};

#endif//__IDIRECT3DDEVICE9_WRAPPER_H