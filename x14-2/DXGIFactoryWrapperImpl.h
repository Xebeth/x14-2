/**************************************************************************
	created		:	2013-04-29
	filename	: 	DXGIFactoryWrapperImpl.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __DXGI_FACTORY_WRAPPER_IMPL_H__
#define __DXGI_FACTORY_WRAPPER_IMPL_H__

class IDXGIFactoryWrapper;

namespace DXGIFactory_vTable
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

		iEnumAdapters,
		iMakeWindowAssociation,
		iGetWindowAssociation,
		iCreateSwapChain,
		iCreateSoftwareAdapter,

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
	/*** IDXGIFactory methods ***/
	typedef HRESULT		(__stdcall IDXGIFactory::*fnEnumAdapters)(UINT, IDXGIAdapter**);
	typedef HRESULT		(__stdcall IDXGIFactory::*fnMakeWindowAssociation)(HWND, UINT);
	typedef HRESULT		(__stdcall IDXGIFactory::*fnGetWindowAssociation)(HWND*);
	typedef HRESULT		(__stdcall IDXGIFactory::*fnCreateSwapChain)(IUnknown*, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**);
	typedef HRESULT		(__stdcall IDXGIFactory::*fnCreateSoftwareAdapter)(HMODULE, IDXGIAdapter**);

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

		fnEnumAdapters pfnEnumAdapters;
		fnMakeWindowAssociation pfnMakeWindowAssociation;
		fnGetWindowAssociation pfnGetWindowAssociation;
		fnCreateSwapChain pfnCreateSwapChain;
		fnCreateSoftwareAdapter pfnCreateSoftwareAdapter;
	} Entries;

	#pragma pack(pop)
	// restore the structure alignment
}

#define CALL_DXGI_FACTORY_VTABLE(Impl, fn) (Impl->m_pDXGIFactory->*Impl->m_OriginalVTable.fn)

class DXGIFactoryWrapperImpl
{
	friend class IDXGIFactoryWrapper;
public:
	DXGIFactoryWrapperImpl(IDXGIFactory *pFactory_in, ISwapChainCreateSubscriber *pSubscriber_in);
	~DXGIFactoryWrapperImpl();

	void OnSwapChainCreate(IDXGISwapChain *pSwapChain_in, DXGI_SWAP_CHAIN_DESC *pDesc) const;
	const IDXGIFactory* GetDXGIFactory() const { return m_pDXGIFactory; }

	void PatchVTable(IDXGIFactory *pFactory_in);
		
private:
	void PatchVTable(bool bRestore_in);

	DXGIFactory_vTable::Entries m_OriginalVTable;
	DXGIFactory_vTable::Entries *m_pHookVTable;
	ISwapChainCreateSubscriber *m_pSubscriber;
	IDXGIFactory *m_pDXGIFactory;
};

#endif//__DXGI_FACTORY_WRAPPER_IMPL_H__
