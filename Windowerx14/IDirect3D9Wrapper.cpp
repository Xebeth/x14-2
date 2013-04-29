#include "stdafx.h"
#include <d3dx9.h>
#include <d3d9.h>

#include "Font.h"
#include "IRenderable.h"
#include "IDirect3D9Wrapper.h"
#include "Direct3D9WrapperImpl.h"
#include "IDirect3DDevice9Wrapper.h"

void IDirect3D9Wrapper::SetImpl(Direct3D9WrapperImpl *pImpl_in, IDirect3D9 *pDirect3D9_in)
{
	m_pDirect3D9 = pDirect3D9_in;
	m_pImpl = pImpl_in;
}

void IDirect3D9Wrapper::DestroyImpl()
{
	if (m_pImpl != NULL)
	{
		delete m_pImpl;
		m_pImpl = NULL;
	}
}

HRESULT __stdcall IDirect3D9Wrapper::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
												  D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
	if (m_pImpl == NULL)
		return m_pDirect3D9->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	LPDIRECT3DDEVICE9 pDirect3dDevice9 = NULL;
	HRESULT Result;

	// force vertical sync
	if (m_pImpl->m_VSync)
		m_pImpl->ForceVSync(*pPresentationParameters);

	// create the Direct3D device
	Result = m_pDirect3D9->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, &pDirect3dDevice9);

	if(Result == D3D_OK && pDirect3dDevice9 != NULL)
	{
		IDirect3DDevice9Wrapper *pWrappedDevice;

		// wrap the device up
		*ppReturnedDeviceInterface = pWrappedDevice = new IDirect3DDevice9Wrapper;
		// notify the subscriber
		m_pImpl->OnDeviceCreate(*pPresentationParameters, pWrappedDevice, pDirect3dDevice9);
	}

	return Result;
}

HRESULT __stdcall IDirect3D9Wrapper::QueryInterface(REFIID iid, void ** ppvObject)
{
	return m_pDirect3D9->QueryInterface(iid, ppvObject);
}

ULONG __stdcall IDirect3D9Wrapper::AddRef(void)
{	
	return m_pDirect3D9->AddRef();
}

ULONG __stdcall IDirect3D9Wrapper::Release(void)
{
	DestroyImpl();

	return m_pDirect3D9->Release();
}

/*** IDirect3D9 methods ***/
HRESULT __stdcall IDirect3D9Wrapper::RegisterSoftwareDevice(void* pInitializeFunction)
{
	return m_pDirect3D9->RegisterSoftwareDevice(pInitializeFunction);
}

UINT    __stdcall IDirect3D9Wrapper::GetAdapterCount()
{
	return m_pDirect3D9->GetAdapterCount();
}

HRESULT  __stdcall IDirect3D9Wrapper::GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9* pIdentifier)
{
	return m_pDirect3D9->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT    __stdcall IDirect3D9Wrapper::GetAdapterModeCount(UINT Adapter,D3DFORMAT Format) 
{
	return m_pDirect3D9->GetAdapterModeCount(Adapter,Format);
}

HRESULT __stdcall IDirect3D9Wrapper::EnumAdapterModes(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE* pMode)
{
	return m_pDirect3D9->EnumAdapterModes(Adapter, Format, Mode, pMode);
}

HRESULT __stdcall IDirect3D9Wrapper::GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE* pMode)
{
	return m_pDirect3D9->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT __stdcall IDirect3D9Wrapper::CheckDeviceType(UINT iAdapter,D3DDEVTYPE DevType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL bWindowed)
{
	return m_pDirect3D9->CheckDeviceType(iAdapter,DevType,DisplayFormat,BackBufferFormat,bWindowed);
}

HRESULT __stdcall IDirect3D9Wrapper::CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat)
{
	return m_pDirect3D9->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat,Usage, RType,CheckFormat);
}

HRESULT __stdcall IDirect3D9Wrapper::CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD* pQualityLevels)
{
	return m_pDirect3D9->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
}

HRESULT __stdcall IDirect3D9Wrapper::CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat)
{
	return m_pDirect3D9->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}

HRESULT __stdcall IDirect3D9Wrapper::CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat)
{
	return m_pDirect3D9->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}

HRESULT __stdcall IDirect3D9Wrapper::GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9* pCaps)
{
	return m_pDirect3D9->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HMONITOR  __stdcall IDirect3D9Wrapper::GetAdapterMonitor(UINT Adapter)
{
	return m_pDirect3D9->GetAdapterMonitor(Adapter);
}