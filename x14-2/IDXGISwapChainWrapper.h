#ifndef __IDXGI_SWAPCHAIN_WRAPPER_H
#define __IDXGI_SWAPCHAIN_WRAPPER_H

class IDXGISwapChainWrapper
{
public:
	HRESULT __stdcall Present(UINT SyncInterval, UINT Flags);
};

#endif//__IDXGI_SWAPCHAIN_WRAPPER_H