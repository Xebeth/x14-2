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

namespace D3D9_vTable
{
	enum 
	{
		iQueryInterface = 0UL,
		iAddRef,
		iRelease,

		iRegisterSoftwareDevice,
		iGetAdapterIdentifier,
		iGetAdapterModeCount,
		iEnumAdapterModes,
		iGetAdapterDisplayMode,
		iCheckDeviceType,
		iCheckDeviceFormat,
		iCheckDeviceMultiSampleType,
		iCheckDepthStencilMatch,
		iCheckDeviceFormatConversion,
		iGetDeviceCaps,
		iGetAdapterMonitor,
		iCreateDevice,
		vTableEntryCount,
	};

	/*** IUnknown methods ***/
	typedef HRESULT		(__stdcall *fnQueryInterface)(REFIID, void**);
	typedef ULONG		(__stdcall *fnAddRef)(void);
	typedef ULONG		(__stdcall *fnRelease)(void);
	/*** IDirect3D9 methods ***/
	typedef HRESULT		(__stdcall IDirect3D9::*fnRegisterSoftwareDevice)(void*);
	typedef UINT		(__stdcall IDirect3D9::*fnGetAdapterCount)();
	typedef HRESULT		(__stdcall IDirect3D9::*fnGetAdapterIdentifier)(UINT,DWORD,D3DADAPTER_IDENTIFIER9*);
	typedef UINT		(__stdcall IDirect3D9::*fnGetAdapterModeCount)(UINT,D3DFORMAT);
	typedef HRESULT		(__stdcall IDirect3D9::*fnEnumAdapterModes)(UINT,D3DFORMAT,UINT,D3DDISPLAYMODE*);
	typedef HRESULT		(__stdcall IDirect3D9::*fnGetAdapterDisplayMode)(UINT,D3DDISPLAYMODE*);
	typedef HRESULT		(__stdcall IDirect3D9::*fnCheckDeviceType)(UINT,D3DDEVTYPE,D3DFORMAT,D3DFORMAT,BOOL);
	typedef HRESULT		(__stdcall IDirect3D9::*fnCheckDeviceFormat)(UINT,D3DDEVTYPE,D3DFORMAT,DWORD,D3DRESOURCETYPE,D3DFORMAT);
	typedef HRESULT		(__stdcall IDirect3D9::*fnCheckDeviceMultiSampleType)(UINT,D3DDEVTYPE,D3DFORMAT,BOOL,D3DMULTISAMPLE_TYPE,DWORD*);
	typedef HRESULT		(__stdcall IDirect3D9::*fnCheckDepthStencilMatch)(UINT,D3DDEVTYPE,D3DFORMAT,D3DFORMAT,D3DFORMAT);
	typedef HRESULT		(__stdcall IDirect3D9::*fnCheckDeviceFormatConversion)(UINT,D3DDEVTYPE,D3DFORMAT,D3DFORMAT);
	typedef HRESULT		(__stdcall IDirect3D9::*fnGetDeviceCaps)(UINT,D3DDEVTYPE,D3DCAPS9*);
	typedef HMONITOR	(__stdcall IDirect3D9::*fnGetAdapterMonitor)(UINT);
	typedef HRESULT		(__stdcall IDirect3D9::*fnCreateDevice)(UINT,D3DDEVTYPE,HWND,DWORD,D3DPRESENT_PARAMETERS*,IDirect3DDevice9**);

	// set the structure alignment to 1 byte
	#pragma pack(push, 1)

	typedef struct _Entries
	{
		fnQueryInterface pfnQueryInterface;
		fnAddRef pfnAddRef;
		fnRelease pfnRelease;

		fnRegisterSoftwareDevice pfnRegisterSoftwareDevice;
		fnGetAdapterCount pfnGetAdapterCount;
		fnGetAdapterIdentifier pfnGetAdapterIdentifier;
		fnGetAdapterModeCount pfnGetAdapterModeCount;
		fnEnumAdapterModes pfnEnumAdapterModes;
		fnGetAdapterDisplayMode pfnGetAdapterDisplayMode;
		fnCheckDeviceType pfnCheckDeviceType;
		fnCheckDeviceFormat pfnCheckDeviceFormat;
		fnCheckDeviceMultiSampleType pfnCheckDeviceMultiSampleType;
		fnCheckDepthStencilMatch pfnCheckDepthStencilMatch;
		fnCheckDeviceFormatConversion pfnCheckDeviceFormatConversion;
		fnGetDeviceCaps pfnGetDeviceCaps;
		fnGetAdapterMonitor pfnGetAdapterMonitor;
		fnCreateDevice pfnCreateDevice;
	} Entries;

	#pragma pack(pop)
	// restore the structure alignment
}

#define CALL_D3D_VTABLE(Impl, fn) (Impl->m_pDirect3D9->*Impl->m_OriginalVTable.fn)

class Direct3D9WrapperImpl
{
	friend class IDirect3D9Wrapper;
public:
	Direct3D9WrapperImpl(IDirect3D9 *pDirect3D9_in, BOOL VSync_in,
						 IDeviceCreateSubscriber* pSubscriber_in);
	~Direct3D9WrapperImpl();
	
	void ForceVSync(D3DPRESENT_PARAMETERS &PresentationParameters_in_out) const;
	void OnDeviceCreate(IDirect3DDevice9 *pDevice_in, const D3DPRESENT_PARAMETERS &PresentParams_in) const;

private:
	void PatchVTable(bool bRestore_in);

	IDeviceCreateSubscriber *m_pSubscriber;
	D3D9_vTable::Entries m_OriginalVTable;
	D3D9_vTable::Entries *m_pHookVTable;
	IDirect3D9Wrapper m_Wrapper;
	IDirect3D9 *m_pDirect3D9;
	const BOOL m_VSync;	
};

#endif//__DIRECT3D9_WRAPPER_IMPL_H__
