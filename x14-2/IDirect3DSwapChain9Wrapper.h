#ifndef __IDIRECT3D9_SWAPCHAIN_WRAPPER_H
#define __IDIRECT3D9_SWAPCHAIN_WRAPPER_H

class IDirect3DSwapChain9Wrapper
{
public:
	HRESULT	__stdcall Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags);
};

#endif//__IDIRECT3D9_SWAPCHAIN_WRAPPER_H