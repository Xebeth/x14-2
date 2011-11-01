/**************************************************************************
	created		:	2010-10-10
	filename	: 	GraphicsCore.cpp
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interface with Direct3D 9
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <HookEngine.h>
#include <d3dx9.h>
#include <d3d9.h>

#include "WindowerSettings.h"

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
	/*! \brief GraphicsCore constructor
		\param[in,out] Engine_in_out : the windower engine
		\param[in] ResX_in : the width of the rendering surface
		\param[in] ResY_in : the height of the rendering surface
		\param[in] VSync_in : flag specifying if vertical synchronization is in use
	*/
	GraphicsCore::GraphicsCore(WindowerEngine &Engine_in_out, LONG ResX_in, LONG ResY_in, BOOL VSync_in)
		: WindowerCore(Engine_in_out), m_ResX(ResX_in), m_ResY(ResY_in), m_VSync(VSync_in)
	{
		m_pDirect3DCreate9Trampoline = Direct3DCreate9;
		m_pDirect3DWrapper = NULL;
		m_pDeviceWrapper = NULL;
	}

	//! \brief GraphicsCore destructor
	GraphicsCore::~GraphicsCore()
	{
		m_pDeviceWrapper = NULL;

		if (m_pDirect3DWrapper != NULL)
		{
			delete m_pDirect3DWrapper;
			m_pDirect3DWrapper = NULL;
		}
	}

	/*! \brief Creates a Direct3D device given a DirectX SDK version
		\param[in] SDKVersion_in : the DirectX SDK version
		\return a pointer to the new device
	*/
	IDirect3D9Wrapper* GraphicsCore::Direct3DCreate9Hook(UINT SDKVersion_in)
	{
		if (m_pDirect3DCreate9Trampoline != NULL)
		{
			IDirect3D9* pDirect3D = m_pDirect3DCreate9Trampoline(SDKVersion_in);

			if (pDirect3D != NULL)
			{
				m_pDirect3DWrapper = new IDirect3D9Wrapper(pDirect3D, m_ResX, m_ResY, m_VSync);
				// subscribe for a pointer to the Direct3DDevice wrapper
				m_pDirect3DWrapper->Subscribe(&m_pDeviceWrapper);
			}
		}

		return m_pDirect3DWrapper;
	}

	//! \brief Switches on/off the rendering added by the windower
	void GraphicsCore::ToggleRendering()
	{
		if (m_pDeviceWrapper != NULL)
			m_pDeviceWrapper->ToggleRendering();
	}

	/*! \brief Callback function invoked when the Direct3D9 device is created
		\param[in] pDeviceWrapper_in : the newly created device
	*/
	void GraphicsCore::OnCreateDevice(IDirect3DDevice9Wrapper *pDeviceWrapper_in)
	{
		m_pDeviceWrapper = pDeviceWrapper_in;
	}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void GraphicsCore::RegisterHooks(IHookManager &HookManager_in)
	{
		// register the Direct3DCreate9 hook
		HookManager_in.RegisterHook("Direct3DCreate9", "d3d9.dll", NULL, ::Direct3DCreate9Hook);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void GraphicsCore::OnHookInstall(IHookManager &HookManager_in)
	{
		m_pDirect3DCreate9Trampoline = (fnDirect3DCreate9)HookManager_in.GetTrampolineFunc("Direct3DCreate9");
	}
}