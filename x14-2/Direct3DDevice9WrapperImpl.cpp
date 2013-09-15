/**************************************************************************
	created		:	2013-04-29
	filename	: 	Direct3DDevice9WrapperImpl.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <d3d9.h>

#include "IRenderable.h"
#include "IDirect3DDevice9Wrapper.h"
#include "Direct3DDevice9WrapperImpl.h"

Direct3DDevice9WrapperImpl::Direct3DDevice9WrapperImpl(IDirect3DDevice9 *pDevice_in, const D3DPRESENT_PARAMETERS &PresentParams_in)
	: m_PresentParams(PresentParams_in), m_pDirect3DDevice(pDevice_in),
	  m_bFullscreen(false), m_bRender(true), m_pHookVTable(NULL)
{
	DWORD vTableSize = D3DDevice9_vTable::vfTableEntryCount * sizeof(LPVOID);

	m_bFullscreen = (PresentParams_in.Windowed == FALSE);
	// retrieve the pointer to the current one
	m_pHookVTable = *reinterpret_cast<D3DDevice9_vTable::Entries**>(pDevice_in);
	// make a copy of the original vTable
	memcpy_s(&m_OriginalVTable, sizeof(m_OriginalVTable), m_pHookVTable, sizeof(m_OriginalVTable));
	// patch the vtable
	PatchVTable(false);
}

Direct3DDevice9WrapperImpl::~Direct3DDevice9WrapperImpl()
{
	// disable rendering
	m_bRender = false;
	// restore the vtable
	PatchVTable(true);
	// reset the pointers
	m_pDirect3DDevice = NULL;
}

void Direct3DDevice9WrapperImpl::PatchVTable(bool bRestore_in)
{
	DWORD vTableSize = D3DDevice9_vTable::vfTableEntryCount * sizeof(LPVOID);
	DWORD dwPageAccess = PAGE_READWRITE;

	// change the page protection since the vtable is in .rdata (read-only)
	if (VirtualProtect(m_pHookVTable, vTableSize, dwPageAccess, &dwPageAccess))
	{
		if (bRestore_in)
		{
			// restore the vTable
			m_pHookVTable->pfnReset						= m_OriginalVTable.pfnReset;
			m_pHookVTable->pfnPresent					= m_OriginalVTable.pfnPresent;
			m_pHookVTable->pfnCreateAdditionalSwapChain = m_OriginalVTable.pfnCreateAdditionalSwapChain;
		}
		else
		{
			SetHooks();
		}
		// restore the page protection
		VirtualProtect(m_pHookVTable, vTableSize, dwPageAccess, &dwPageAccess);
	}
}

bool Direct3DDevice9WrapperImpl::AddRenderable(unsigned long ID_in, IRenderable *pRenderable_in)
{
	if (pRenderable_in != NULL)
	{
		m_UiElements[ID_in] = pRenderable_in;

		return true;
	}

	return false;
}

bool Direct3DDevice9WrapperImpl::RemoveRenderable(unsigned long ID_in)
{
	RenderableMap::const_iterator RenderableIt = m_UiElements.find(ID_in);

	if (RenderableIt != m_UiElements.end())
	{
		m_UiElements.erase(RenderableIt);
		
		return true;
	}
	
	return false;
}

void Direct3DDevice9WrapperImpl::Draw()
{
	if (m_bRender)
	{
		RenderableMap::const_iterator RenderableIt, EndIt = m_UiElements.cend();
		IRenderable *pRenderable = NULL;

		for(RenderableIt = m_UiElements.cbegin(); RenderableIt != EndIt; ++RenderableIt)
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

void Direct3DDevice9WrapperImpl::OnDeviceLost(D3DPRESENT_PARAMETERS &PresentationParameters_in_out)
{
	RenderableMap::const_iterator RenderableIt, EndIt = m_UiElements.cend();
	IRenderable *pRenderable = NULL;

	m_bRender = false;
	// force vertical sync
	PresentationParameters_in_out.PresentationInterval = m_PresentParams.PresentationInterval;
	PresentationParameters_in_out.BackBufferCount = m_PresentParams.BackBufferCount;
	PresentationParameters_in_out.SwapEffect = m_PresentParams.SwapEffect;
	m_bFullscreen = (PresentationParameters_in_out.Windowed == FALSE);
	// update the vTable
	PatchVTable(false);

	for(RenderableIt = m_UiElements.cbegin(); RenderableIt != EndIt; ++RenderableIt)
	{
		pRenderable = RenderableIt->second;

		if (pRenderable != NULL)
			pRenderable->OnDeviceLost();
	}
}

void Direct3DDevice9WrapperImpl::OnDeviceReset()
{
	RenderableMap::const_iterator RenderableIt, EndIt = m_UiElements.cend();
	IRenderable *pRenderable = NULL;

	for(RenderableIt = m_UiElements.cbegin(); RenderableIt != EndIt; ++RenderableIt)
	{
		pRenderable = RenderableIt->second;

		if (pRenderable != NULL)
			pRenderable->OnDeviceReset();
	}

	m_bRender = true;
}

void Direct3DDevice9WrapperImpl::SetHooks()
{
	// hook the functions
	m_pHookVTable->pfnReset						= (D3DDevice9_vTable::fnReset)&IDirect3DDevice9Wrapper::Reset;
	m_pHookVTable->pfnPresent					= (D3DDevice9_vTable::fnPresent)&IDirect3DDevice9Wrapper::Present;
	m_pHookVTable->pfnCreateAdditionalSwapChain = (D3DDevice9_vTable::fnCreateAdditionalSwapChain)&IDirect3DDevice9Wrapper::CreateAdditionalSwapChain;
}