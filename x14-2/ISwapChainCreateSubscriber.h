/**************************************************************************
created		:	2013-04-29
filename	: 	ISwapChainCreateSubscriber.h
author		:	Xebeth`
copyright	:	North Edge (2013)
purpose		:	DXGISwapChain creation subscriber
**************************************************************************/
#ifndef __I_DXGI_SWAPCHAIN_CREATE_SUBSCRIBER_H__
#define __I_DXGI_SWAPCHAIN_CREATE_SUBSCRIBER_H__

typedef struct DXGI_SWAP_CHAIN_DESC DXGI_SWAP_CHAIN_DESC;
typedef struct IDXGISwapChain IDXGISwapChain;

class ISwapChainCreateSubscriber
{
public:
	virtual void OnSwapChainCreate(IDXGISwapChain *pFactory_in, const DXGI_SWAP_CHAIN_DESC *pDesc_in) = 0;
};

#endif//__I_DXGI_SWAPCHAIN_CREATE_SUBSCRIBER_H__
