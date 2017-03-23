/**************************************************************************
	created		:	2013-04-29
	filename	: 	DXGISwapChainWrapperImpl.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __DIRECT3D11_SWAPCHAIN_WRAPPER_IMPL_H__
#define __DIRECT3D11_SWAPCHAIN_WRAPPER_IMPL_H__

class IDXGISwapChainWrapper;

namespace DXGISwapChain_vTable
{
	enum 
	{
		iQueryInterface = 0UL,
		iAddRef,
		iRelease,

		iSetPrivateData,
		iSetPrivateDataInterface,
		iGetPrivateData,
		iGetParent,
		iGetDevice,
		iPresent,
		iGetBuffer,
		iSetFullscreenState,
		iGetFullscreenState,
		iGetDesc,
		iResizeBuffers,
		iResizeTarget,
		iGetContainingOutput,
		iGetFrameStatistics,
		iGetLastPresentCount,

		vTableEntryCount,
	};

	/*** IUnknown methods ***/
	typedef HRESULT		(__stdcall IUnknown::*fnQueryInterface)(REFIID, void**);
	typedef ULONG		(__stdcall IUnknown::*fnAddRef)(void);
	typedef ULONG		(__stdcall IUnknown::*fnRelease)(void);
	/*** IDXGIObject methods ***/
	typedef HRESULT		(__stdcall IDXGIObject::*fnSetPrivateData)(REFGUID, UINT, const void*);
	typedef HRESULT		(__stdcall IDXGIObject::*fnSetPrivateDataInterface)(REFGUID, const IUnknown*);
	typedef HRESULT		(__stdcall IDXGIObject::*fnGetPrivateData)(REFGUID, UINT*, void*);
	typedef HRESULT		(__stdcall IDXGIObject::*fnGetParent)(REFIID riid, void**);
	typedef HRESULT		(__stdcall IDXGIObject::*fnGetDevice)(REFIID riid, void**);
	/*** IDXGISwapChain methods ***/
	typedef HRESULT		(__stdcall IDXGISwapChain::*fnPresent)(UINT, UINT);
	typedef HRESULT		(__stdcall IDXGISwapChain::*fnGetBuffer)(UINT, REFIID, void**);
	typedef HRESULT		(__stdcall IDXGISwapChain::*fnSetFullscreenState)(BOOL, IDXGIOutput*);
	typedef HRESULT		(__stdcall IDXGISwapChain::*fnGetFullscreenState)(BOOL*, IDXGIOutput**);
	typedef HRESULT		(__stdcall IDXGISwapChain::*fnGetDesc)(DXGI_SWAP_CHAIN_DESC*);
	typedef HRESULT		(__stdcall IDXGISwapChain::*fnResizeBuffers)(UINT, UINT, UINT, DXGI_FORMAT, UINT);
	typedef HRESULT		(__stdcall IDXGISwapChain::*fnResizeTarget)(const DXGI_MODE_DESC*);
	typedef HRESULT		(__stdcall IDXGISwapChain::*fnGetContainingOutput)(IDXGIOutput**);
	typedef HRESULT		(__stdcall IDXGISwapChain::*fnGetFrameStatistics)(DXGI_FRAME_STATISTICS*);
	typedef HRESULT		(__stdcall IDXGISwapChain::*fnGetLastPresentCount)(UINT*);

	// set the structure alignment to 1 byte
	#pragma pack(push, 1)

	typedef struct _Entries
	{
		fnQueryInterface pfnQueryInterface;
		fnAddRef pfnAddRef;
		fnRelease pfnRelease;

		fnSetPrivateData pfnSetPrivateData;
		fnSetPrivateDataInterface pfnSetPrivateDataInterface;
		fnGetPrivateData pfnGetPrivateData;
		fnGetParent pfnGetParent;
		fnGetDevice pfnGetDevice;
		fnPresent pfnPresent;
		fnGetBuffer pfnGetBuffer;
		fnSetFullscreenState pfnSetFullscreenState;
		fnGetFullscreenState pfnGetFullscreenState;
		fnGetDesc pfnGetDesc;
		fnResizeBuffers pfnResizeBuffers;
		fnResizeTarget pfnResizeTarget;
		fnGetContainingOutput pfnGetContainingOutput;
		fnGetFrameStatistics pfnGetFrameStatistics;
		fnGetLastPresentCount pfnGetLastPresentCount;
	} Entries;

	#pragma pack(pop)
	// restore the structure alignment
}

#define CALL_DXGI_SWAPCHAIN_VTABLE(Impl, fn) (Impl->m_pDXGISwapChain11->*Impl->m_OriginalVTable.fn)

class DXGISwapChainWrapperImpl
{
	friend class IDXGISwapChainWrapper;
public:
	DXGISwapChainWrapperImpl(IDXGISwapChain *pDirect3DSwapChain11_in,
							 const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc_in);
	~DXGISwapChainWrapperImpl();

	void SetRendering(bool bEnable_in) { m_bRender = bEnable_in; }
	bool IsFullscreen() const { return m_bFullscreen; }
	void ToggleRendering()	{ m_bRender = !m_bRender; }
	bool IsRendering() const { return m_bRender; }

	void Draw();
		
private:
	void PatchVTable(bool bRestore_in);

	DXGISwapChain_vTable::Entries m_OriginalVTable;
	DXGISwapChain_vTable::Entries *m_pHookVTable;
	IDXGISwapChain *m_pDXGISwapChain11;
	RenderableMap m_UiElements;
	bool m_bFullscreen;
	bool m_bRender;
};

#endif//__DIRECT3D11_SWAPCHAIN_WRAPPER_IMPL_H__