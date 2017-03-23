/**************************************************************************
	created		:	2013-04-29
	filename	: 	Direct3DSwapChain9WrapperImpl.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <d3d9.h>

#include "IDirect3DSwapChain9Wrapper.h"
#include "Direct3DSwapChain9WrapperImpl.h"
#include "IDeviceCreateSubscriber.h"

Direct3DSwapChain9WrapperImpl::Direct3DSwapChain9WrapperImpl(IDirect3DSwapChain9 *pDirect3DSwapChain9_in)
	: m_pDirect3DSwapChain9(pDirect3DSwapChain9_in), m_pHookVTable(nullptr)
{
	// retrieve the pointer to the current one
	m_pHookVTable = *reinterpret_cast<D3DSwapChain9_vTable::Entries**>(pDirect3DSwapChain9_in);
	// make a copy of the original vTable
	memcpy_s(&m_OriginalVTable, sizeof(m_OriginalVTable), m_pHookVTable, sizeof(m_OriginalVTable));
	// patch the vtable
	PatchVTable(false);
}

Direct3DSwapChain9WrapperImpl::~Direct3DSwapChain9WrapperImpl()
{
	// restore the vtable
	PatchVTable(true);
	// reset the pointers
	m_pHookVTable = nullptr;
	m_pDirect3DSwapChain9 = nullptr;
}

void Direct3DSwapChain9WrapperImpl::PatchVTable(bool bRestore_in)
{
	DWORD vTableSize = D3DSwapChain9_vTable::vTableEntryCount * sizeof(LPVOID);
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
			m_pHookVTable->pfnPresent = (D3DSwapChain9_vTable::fnPresent)&IDirect3DSwapChain9Wrapper::Present;
		}
		// restore the page protection
		VirtualProtect(m_pHookVTable, vTableSize, dwPageAccess, &dwPageAccess);
	}
}

void Direct3DSwapChain9WrapperImpl::OnDeviceLost()
{
	if (m_pDirect3DSwapChain9 != nullptr)
	{
		m_pDirect3DSwapChain9->Release();
		m_pDirect3DSwapChain9 = nullptr;
	}
}