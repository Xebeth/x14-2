#ifndef __IDIRECT3D9_WRAPPER_H
#define __IDIRECT3D9_WRAPPER_H

class IDirect3DDevice9Wrapper;

typedef std::vector<IDirect3DDevice9Wrapper**> DeviceSubscribers;

class IDirect3D9Wrapper : public IDirect3D9
{
public:
	IDirect3D9Wrapper(LPDIRECT3D9 pDirect3D9_in, LONG ResX_in, LONG ResY_in, BOOL VSync_in); 
	virtual ~IDirect3D9Wrapper();

	IDirect3DDevice9Wrapper* GetDevice() { return m_pWrappedDevice; }
	void Subscribe(IDirect3DDevice9Wrapper** pDevicePtr_in);

	//IDirect3D9 Interface Methods
	HRESULT __stdcall QueryInterface(REFIID iid, void ** ppvObject);
	ULONG	__stdcall AddRef(void);
	ULONG	__stdcall Release(void);

	/*** IDirect3D9 methods ***/
	HRESULT __stdcall RegisterSoftwareDevice(void* pInitializeFunction) ;
	UINT    __stdcall GetAdapterCount();
	HRESULT __stdcall GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9* pIdentifier) ;
	UINT    __stdcall GetAdapterModeCount(UINT Adapter,D3DFORMAT Format) ;
	HRESULT __stdcall EnumAdapterModes(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE* pMode) ;
	HRESULT __stdcall GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE* pMode) ;
	HRESULT __stdcall CheckDeviceType(UINT iAdapter,D3DDEVTYPE DevType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL bWindowed) ;
	HRESULT __stdcall CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat) ;
	HRESULT __stdcall CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD* pQualityLevels) ;
	HRESULT __stdcall CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat) ;
	HRESULT __stdcall CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat) ;
	HRESULT __stdcall GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9* pCaps) ;
	HMONITOR  __stdcall GetAdapterMonitor(UINT Adapter) ;
	HRESULT __stdcall CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface) ;

	//Internal Method and member variables
protected:
	IDirect3DDevice9Wrapper		*m_pWrappedDevice;
	IDirect3D9					*m_pDirect3D9;
	HWND						 m_hGameWnd;

	DeviceSubscribers			 m_Subscribers;

	// Device creation parameters
	//! the width of the rendering surface
	const LONG m_ResX;
	//! the height of the rendering surface
	const LONG m_ResY;
	//! flag specifying if vertical synchronization is in use
	const BOOL m_VSync;
};

#endif//__IDIRECT3D9_WRAPPER_H