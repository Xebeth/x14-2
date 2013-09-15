#ifndef __IDIRECT3DDEVICE9_WRAPPER_H
#define __IDIRECT3DDEVICE9_WRAPPER_H

class IDirect3DDevice9Wrapper
{
public:
	HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* pPresentationParameters);
	HRESULT __stdcall Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);
	HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9** pSwapChain);

};

#endif//__IDIRECT3DDEVICE9_WRAPPER_H