/**************************************************************************
	created		:	2013-04-29
	filename	: 	Direct3D9WrapperImpl.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <d3d9.h>

#include "IDirect3D9Wrapper.h"
#include "Direct3D9WrapperImpl.h"
#include "IDeviceCreateSubscriber.h"

Direct3D9WrapperImpl::Direct3D9WrapperImpl(IDirect3D9 *pDirect3D9_in, BOOL VSync_in,
										   IDeviceCreateSubscriber* pSubscriber_in)
	: m_VSync(0), m_pSubscriber(pSubscriber_in),
	  m_pDirect3D9(pDirect3D9_in), m_pHookVTable(NULL)
{
	// retrieve the pointer to the current one
	m_pHookVTable = *reinterpret_cast<D3D9_vTable::Entries**>(pDirect3D9_in);
	// make a copy of the original vTable
	memcpy_s(&m_OriginalVTable, sizeof(m_OriginalVTable), m_pHookVTable, sizeof(m_OriginalVTable));
	// patch the vtable
	PatchVTable(false);
}

Direct3D9WrapperImpl::~Direct3D9WrapperImpl()
{
	// restore the vtable
	PatchVTable(true);
	// reset the pointers
	m_pHookVTable = NULL;
	m_pDirect3D9 = NULL;
}

void Direct3D9WrapperImpl::PatchVTable(bool bRestore_in)
{
	DWORD vTableSize = D3D9_vTable::vTableEntryCount * sizeof(LPVOID);
	DWORD dwPageAccess = PAGE_READWRITE;

	// change the page protection since the vtable is in .rdata (read-only)
	if (VirtualProtect(m_pHookVTable, vTableSize, dwPageAccess, &dwPageAccess))
	{
		if (bRestore_in)
		{
			// restore the vTable
			m_pHookVTable->pfnCreateDevice = m_OriginalVTable.pfnCreateDevice;
		}
		else
		{
			// hook the CreateDevice function
			m_pHookVTable->pfnCreateDevice = (D3D9_vTable::fnCreateDevice)&IDirect3D9Wrapper::CreateDevice;
		}
		// restore the page protection
		VirtualProtect(m_pHookVTable, vTableSize, dwPageAccess, &dwPageAccess);
	}
}

void Direct3D9WrapperImpl::ForceVSync(D3DPRESENT_PARAMETERS &PresentationParameters_in_out) const
{
	// force vertical sync
	if (m_VSync)
	{
		PresentationParameters_in_out.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		PresentationParameters_in_out.SwapEffect = D3DSWAPEFFECT_DISCARD;
		PresentationParameters_in_out.BackBufferCount = 2;
	}
}

void Direct3D9WrapperImpl::OnDeviceCreate(IDirect3DDevice9 *pDevice_in, const D3DPRESENT_PARAMETERS &PresentParams_in) const
{
	if (m_pSubscriber != NULL)
		m_pSubscriber->OnDeviceCreate(pDevice_in, PresentParams_in);
}