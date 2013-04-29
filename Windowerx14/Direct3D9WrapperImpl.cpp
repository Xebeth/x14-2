/**************************************************************************
	created		:	2013-04-29
	filename	: 	Direct3D9WrapperImpl.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <d3d9.h>

#include "Direct3D9WrapperImpl.h"
#include "IDeviceCreateSubscriber.h"

Direct3D9WrapperImpl::Direct3D9WrapperImpl(BOOL VSync_in, IDeviceCreateSubscriber* pSubscriber_in)
	: m_VSync(VSync_in), m_pSubscriber(pSubscriber_in) {}

void Direct3D9WrapperImpl::ForceVSync(D3DPRESENT_PARAMETERS &PresentationParameters_in_out) const
{
	// force vertical sync
	if (m_VSync)
	{
		PresentationParameters_in_out.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		PresentationParameters_in_out.SwapEffect = D3DSWAPEFFECT_DISCARD;
		PresentationParameters_in_out.BackBufferCount = 2;
	}
}

void Direct3D9WrapperImpl::OnDeviceCreate(const D3DPRESENT_PARAMETERS &PresentParams_in,
										  IDirect3DDevice9Wrapper *pWrappedDevice_in,
										  IDirect3DDevice9 *pDevice_in) const
{
	if (m_pSubscriber != NULL)
		m_pSubscriber->OnDeviceCreate(PresentParams_in, pWrappedDevice_in, pDevice_in);
}