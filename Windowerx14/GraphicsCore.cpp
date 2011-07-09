/**************************************************************************
	created		:	2010-10-10
	filename	: 	GraphicsCore.cpp
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interface with Direct3D 9
**************************************************************************/
#include "stdafx.h"
#include <d3dx9.h>
#include <d3d9.h>

#include <PluginFramework.h>
#include <NonCopyable.h>
#include <HookEngine.h>

#include "BaseEngine.h"
#include "PluginEngine.h"
#include "WindowerEngine.h"

#include "Direct3D9Hook.h"

#include "ICoreModule.h"
#include "WindowerCore.h"

#include "Font.h"
#include "IDirect3D9Wrapper.h"
#include "IDirect3DDevice9Wrapper.h"
#include "Direct3D9Hook.h"

#include "GraphicsCore.h"

namespace Windower
{
	GraphicsCore::GraphicsCore(WindowerEngine &Engine_in, LONG ResX, LONG ResY, BOOL VSync)
		: WindowerCore(Engine_in), m_ResX(ResX), m_ResY(ResY), m_VSync(VSync)
	{
		m_pDirect3DCreate9Trampoline = Direct3DCreate9;
		m_pDirect3DWrapper = NULL;
		m_pDeviceWrapper = NULL;
	}

	/*! \brief 
	*/
	GraphicsCore::~GraphicsCore()
	{
		m_pDeviceWrapper = NULL;

		if (m_pDirect3DWrapper != NULL)
		{
			delete m_pDirect3DWrapper;
			m_pDirect3DWrapper = NULL;
		}
	}

	IDirect3D9Wrapper* GraphicsCore::Direct3DCreate9Hook(UINT SDKVersion)
	{
		if (m_pDirect3DCreate9Trampoline != NULL)
		{
			IDirect3D9* pDirect3D = m_pDirect3DCreate9Trampoline(SDKVersion);

			if (pDirect3D != NULL)
			{
				m_pDirect3DWrapper = new IDirect3D9Wrapper(pDirect3D, m_ResX, m_ResY, m_VSync);
				// subscribe for a pointer to the Direct3DDevice wrapper
				m_pDirect3DWrapper->Subscribe(&m_pDeviceWrapper);
			}
		}

		return m_pDirect3DWrapper;
	}

	/*! \brief Switches on/off the rendering added by the windower */
	void GraphicsCore::ToggleRendering()
	{
		if (m_pDeviceWrapper != NULL)
			m_pDeviceWrapper->ToggleRendering();
	}

	/*! \brief 
	*/
	void GraphicsCore::OnCreateDevice(IDirect3DDevice9Wrapper *pDeviceWrapper)
	{
		m_pDeviceWrapper = pDeviceWrapper;
	}

	/*! \brief 
		\param[] pHookManager : 
	*/
	void GraphicsCore::RegisterHooks(IHookManager *pHookManager)
	{
		if (pHookManager != NULL)
		{
			// register the Direct3DCreate9 hook
			pHookManager->RegisterHook("Direct3DCreate9", "d3d9.dll", NULL, ::Direct3DCreate9Hook);
		}
	}

	/*! \brief 
		\param[] pHookManager : 
	*/
	void GraphicsCore::OnHookInstall(IHookManager *pHookManager)
	{
		if (pHookManager != NULL)
		{
			m_pDirect3DCreate9Trampoline = (fnDirect3DCreate9)pHookManager->GetTrampolineFunc("Direct3DCreate9");
		}
	}
}