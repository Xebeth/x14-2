/**************************************************************************
created		:	2013-04-29
filename	: 	DXGIFactoryWrapperImpl.cpp
author		:	Xebeth`
copyright	:	North Edge (2013)
purpose		:
**************************************************************************/
#include "stdafx.h"
#include <d3d11.h>

#include "IRenderable.h"
#include "IDXGIFactoryWrapper.h"
#include "DXGIFactoryWrapperImpl.h"
#include "ISwapChainCreateSubscriber.h"

DXGIFactoryWrapperImpl::DXGIFactoryWrapperImpl(IDXGIFactory *pFactory_in, ISwapChainCreateSubscriber *pSubscriber_in)
	: m_pDXGIFactory(pFactory_in), m_pHookVTable(nullptr), m_pSubscriber(pSubscriber_in)
{
	// retrieve the pointer to the current one
	m_pHookVTable = *reinterpret_cast<DXGIFactory_vTable::Entries**>(pFactory_in);
	// make a copy of the original vTable
	memcpy_s(&m_OriginalVTable, sizeof(m_OriginalVTable), m_pHookVTable, sizeof(m_OriginalVTable));
	// patch the vtable
	PatchVTable(false);
}

DXGIFactoryWrapperImpl::~DXGIFactoryWrapperImpl()
{
	// restore the vtable
	PatchVTable(true);
	// reset the pointers
	m_pHookVTable = nullptr;
	m_pDXGIFactory = nullptr;
	m_pSubscriber = nullptr;
}

void DXGIFactoryWrapperImpl::PatchVTable(bool bRestore_in)
{
	DWORD vTableSize = DXGIFactory_vTable::vTableEntryCount * sizeof(LPVOID);
	DWORD dwPageAccess = PAGE_READWRITE;

	// change the page protection since the vtable is in .rdata (read-only)
	if (VirtualProtect(m_pHookVTable, vTableSize, dwPageAccess, &dwPageAccess))
	{
		if (bRestore_in)
		{
			// restore the vTable
			m_pHookVTable->pfnEnumAdapters			= m_OriginalVTable.pfnEnumAdapters;
			m_pHookVTable->pfnMakeWindowAssociation = m_OriginalVTable.pfnMakeWindowAssociation;
			m_pHookVTable->pfnGetWindowAssociation	= m_OriginalVTable.pfnGetWindowAssociation;
			m_pHookVTable->pfnCreateSwapChain		= m_OriginalVTable.pfnCreateSwapChain;
			m_pHookVTable->pfnCreateSoftwareAdapter = m_OriginalVTable.pfnCreateSoftwareAdapter;
		}
		else
		{
			// hook the CreateDevice function
			m_pHookVTable->pfnEnumAdapters			= (DXGIFactory_vTable::fnEnumAdapters)&IDXGIFactoryWrapper::EnumAdapters;
			m_pHookVTable->pfnMakeWindowAssociation = (DXGIFactory_vTable::fnMakeWindowAssociation)&IDXGIFactoryWrapper::CreateSwapChain;
			m_pHookVTable->pfnGetWindowAssociation	= (DXGIFactory_vTable::fnGetWindowAssociation)&IDXGIFactoryWrapper::GetWindowAssociation;
			m_pHookVTable->pfnCreateSwapChain		= (DXGIFactory_vTable::fnCreateSwapChain)&IDXGIFactoryWrapper::CreateSwapChain;
			m_pHookVTable->pfnCreateSoftwareAdapter = (DXGIFactory_vTable::fnCreateSoftwareAdapter)&IDXGIFactoryWrapper::CreateSoftwareAdapter;
		}
		// restore the page protection
		VirtualProtect(m_pHookVTable, vTableSize, dwPageAccess, &dwPageAccess);
	}
}

void DXGIFactoryWrapperImpl::PatchVTable(IDXGIFactory *pFactory_in)
{
	DXGIFactory_vTable::Entries *pVtable = *(DXGIFactory_vTable::Entries **)pFactory_in;
	DWORD vTableSize = DXGIFactory_vTable::vTableEntryCount * sizeof(LPVOID);
	DWORD dwPageAccess = PAGE_READWRITE;

	m_pDXGIFactory = pFactory_in;

	// change the page protection since the vtable is in .rdata (read-only)
	if (VirtualProtect(pVtable, vTableSize, dwPageAccess, &dwPageAccess))
	{
		pVtable->pfnEnumAdapters = (DXGIFactory_vTable::fnEnumAdapters)&IDXGIFactoryWrapper::EnumAdapters;
		pVtable->pfnMakeWindowAssociation = (DXGIFactory_vTable::fnMakeWindowAssociation)&IDXGIFactoryWrapper::CreateSwapChain;
		pVtable->pfnGetWindowAssociation = (DXGIFactory_vTable::fnGetWindowAssociation)&IDXGIFactoryWrapper::GetWindowAssociation;
		pVtable->pfnCreateSwapChain = (DXGIFactory_vTable::fnCreateSwapChain)&IDXGIFactoryWrapper::CreateSwapChain;
		pVtable->pfnCreateSoftwareAdapter = (DXGIFactory_vTable::fnCreateSoftwareAdapter)&IDXGIFactoryWrapper::CreateSoftwareAdapter;

		// restore the page protection
		VirtualProtect(pVtable, vTableSize, dwPageAccess, &dwPageAccess);
	}
}

void DXGIFactoryWrapperImpl::OnSwapChainCreate(IDXGISwapChain *pSwapChain_in, DXGI_SWAP_CHAIN_DESC *pDesc) const
{
	if (m_pSubscriber != nullptr)
		m_pSubscriber->OnSwapChainCreate(pSwapChain_in, pDesc);
}