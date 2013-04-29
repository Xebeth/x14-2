/**************************************************************************
	created		:	2013-04-29
	filename	: 	Direct3D9WrapperImpl.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __DIRECT3D9_WRAPPER_IMPL_H__
#define __DIRECT3D9_WRAPPER_IMPL_H__

class IDirect3DDevice9Wrapper;
class IDeviceCreateSubscriber;

class Direct3D9WrapperImpl
{
	friend class IDirect3D9Wrapper;
public:
	Direct3D9WrapperImpl(BOOL VSync_in, IDeviceCreateSubscriber* pSubscriber_in); 
	
	void ForceVSync(D3DPRESENT_PARAMETERS &PresentationParameters_in_out) const;
	void OnDeviceCreate(const D3DPRESENT_PARAMETERS &PresentParams_in,
						IDirect3DDevice9Wrapper *pWrappedDevice_in,
						IDirect3DDevice9 *pDevice_in) const;

protected:
	IDeviceCreateSubscriber *m_pSubscriber;
	//! flag specifying if vertical synchronization is in use
	const BOOL m_VSync;
};

#endif//__DIRECT3D9_WRAPPER_IMPL_H__
