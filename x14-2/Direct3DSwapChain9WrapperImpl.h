/**************************************************************************
	created		:	2013-04-29
	filename	: 	Direct3DSwapChain9WrapperImpl.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __DIRECT3D9_SWAPCHAIN_WRAPPER_IMPL_H__
#define __DIRECT3D9_SWAPCHAIN_WRAPPER_IMPL_H__

class IDirect3DSwapChain9Wrapper;

namespace D3DSwapChain9_vTable
{
	enum 
	{
		iQueryInterface = 0UL,
		iAddRef,
		iRelease,

		iPresent,
		iGetFrontBufferData,
		iGetBackBuffer,
		iGetRasterStatus,
		iGetDisplayMode,
		iGetDevice,
		iGetPresentParameters,

		vTableEntryCount,
	};

	/*** IUnknown methods ***/
	typedef HRESULT		(__stdcall IUnknown::*fnQueryInterface)(REFIID, void**);
	typedef ULONG		(__stdcall IUnknown::*fnAddRef)(void);
	typedef ULONG		(__stdcall IUnknown::*fnRelease)(void);
	/*** IDirect3DSwapChain9 methods ***/
	typedef HRESULT		(__stdcall IDirect3DSwapChain9::*fnPresent)(CONST RECT*,CONST RECT*,HWND,CONST RGNDATA*,DWORD);
	typedef UINT		(__stdcall IDirect3DSwapChain9::*fnGetFrontBufferData)(IDirect3DSurface9*);
	typedef HRESULT		(__stdcall IDirect3DSwapChain9::*fnGetBackBuffer)(UINT,D3DBACKBUFFER_TYPE,IDirect3DSurface9**);
	typedef UINT		(__stdcall IDirect3DSwapChain9::*fnGetRasterStatus)(D3DRASTER_STATUS*);
	typedef HRESULT		(__stdcall IDirect3DSwapChain9::*fnGetDisplayMode)(D3DDISPLAYMODE*);
	typedef HRESULT		(__stdcall IDirect3DSwapChain9::*fnGetDevice)(IDirect3DDevice9**);
	typedef HRESULT		(__stdcall IDirect3DSwapChain9::*fnGetPresentParameters)(D3DPRESENT_PARAMETERS*);

	// set the structure alignment to 1 byte
	#pragma pack(push, 1)

	typedef struct _Entries
	{
		fnQueryInterface pfnQueryInterface;
		fnAddRef pfnAddRef;
		fnRelease pfnRelease;

		fnPresent pfnPresent;
		fnGetFrontBufferData pfnGetFrontBufferData;
		fnGetBackBuffer pfnGetBackBuffer;
		fnGetRasterStatus pfnGetRasterStatus;
		fnGetDisplayMode pfnGetDisplayMode;
		fnGetDevice pfnGetDevice;
		fnGetPresentParameters pfnGetPresentParameters;
	} Entries;

	#pragma pack(pop)
	// restore the structure alignment
}

#define CALL_D3DSWAPCHAIN_VTABLE(Impl, fn) (Impl->m_pDirect3DSwapChain9->*Impl->m_OriginalVTable.fn)

class Direct3DSwapChain9WrapperImpl
{
	friend class IDirect3DSwapChain9Wrapper;
public:
	explicit Direct3DSwapChain9WrapperImpl(IDirect3DSwapChain9 *pDirect3DSwapChain9_in);
	~Direct3DSwapChain9WrapperImpl();

	void OnDeviceLost();
	
private:
	void PatchVTable(bool bRestore_in);

	D3DSwapChain9_vTable::Entries m_OriginalVTable;
	D3DSwapChain9_vTable::Entries *m_pHookVTable;
	IDirect3DSwapChain9 *m_pDirect3DSwapChain9;
};

#endif//__DIRECT3D9_SWAPCHAIN_WRAPPER_IMPL_H__