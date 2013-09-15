/**************************************************************************
	created		:	2013-04-29
	filename	: 	IDeviceCreateSubscriber.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Direct3DDevice9 creation subscriber
**************************************************************************/
#ifndef __I_DEVICE_CREATE_SUBSCRIBER_H__
#define __I_DEVICE_CREATE_SUBSCRIBER_H__

typedef struct _D3DPRESENT_PARAMETERS_ D3DPRESENT_PARAMETERS;
typedef struct IDirect3DDevice9 IDirect3DDevice9;

class IDeviceCreateSubscriber
{
public:
	virtual void OnDeviceCreate(IDirect3DDevice9 *pDevice_in, const D3DPRESENT_PARAMETERS *pPresentParams_in, HWND hWnd_in) =0;
};

#endif//__I_DEVICE_CREATE_SUBSCRIBER_H__
