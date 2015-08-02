/**************************************************************************
	created		:	2013-04-29
	filename	: 	DXGISwapChainWrapperImpl.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <d3d11.h>

#include "IRenderable.h"
#include "IDXGISwapChainWrapper.h"
#include "DXGISwapChainWrapperImpl.h"
#include "IDeviceCreateSubscriber.h"

DXGISwapChainWrapperImpl::DXGISwapChainWrapperImpl(IDXGISwapChain *pSwapChain_in, DXGI_SWAP_CHAIN_DESC *pSwapChainDesc_in)
	: m_pDXGISwapChain11(pSwapChain_in), m_pHookVTable(NULL)
{
	// retrieve the pointer to the current one
	m_pHookVTable = *reinterpret_cast<DXGISwapChain_vTable::Entries**>(pSwapChain_in);
	// make a copy of the original vTable
	memcpy_s(&m_OriginalVTable, sizeof(m_OriginalVTable), m_pHookVTable, sizeof(m_OriginalVTable));
	// patch the vtable
	PatchVTable(false);
}

DXGISwapChainWrapperImpl::~DXGISwapChainWrapperImpl()
{
	// restore the vtable
	PatchVTable(true);
	// reset the pointers
	m_pHookVTable = NULL;
	m_pDXGISwapChain11 = NULL;
}

void DXGISwapChainWrapperImpl::PatchVTable(bool bRestore_in)
{
	DWORD vTableSize = DXGISwapChain_vTable::vTableEntryCount * sizeof(LPVOID);
	DWORD dwPageAccess = PAGE_READWRITE;

	// change the page protection since the vtable is in .rdata (read-only)
	if (VirtualProtect(m_pHookVTable, vTableSize, dwPageAccess, &dwPageAccess))
	{
		if (bRestore_in)
		{
			// restore the vTable
			m_pHookVTable->pfnPresent = m_OriginalVTable.pfnPresent;
		}
		else
		{
			// hook the CreateDevice function
			m_pHookVTable->pfnPresent = (DXGISwapChain_vTable::fnPresent)&IDXGISwapChainWrapper::Present;
		}
		// restore the page protection
		VirtualProtect(m_pHookVTable, vTableSize, dwPageAccess, &dwPageAccess);
	}
}

void DXGISwapChainWrapperImpl::Draw()
{
	if (m_bRender)
	{
		RenderableMap::const_iterator RenderableIt, EndIt = m_UiElements.cend();
		IRenderable *pRenderable = NULL;

		for (RenderableIt = m_UiElements.cbegin(); RenderableIt != EndIt; ++RenderableIt)
		{
			pRenderable = RenderableIt->second;

			if (pRenderable != NULL && pRenderable->IsVisible())
			{
				pRenderable->Update();
				pRenderable->Draw();
			}
		}
	}
}