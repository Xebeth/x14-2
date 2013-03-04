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

#include <CommandHandler.h>
#include "WindowerSettings.h"
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
		\param[in] VSync_in : flag specifying if vertical synchronization is in use
	*/
	GraphicsCore::GraphicsCore(WindowerEngine &Engine_in_out, BOOL VSync_in)
		: WindowerCore(Engine_in_out), m_VSync(VSync_in)
	{
		m_pDirect3DCreate9Trampoline = Direct3DCreate9;
		m_Engine.RegisterModule(_T("Graphics"), this);
	}

	//! \brief GraphicsCore destructor
	GraphicsCore::~GraphicsCore()
	{
		Direct3DWrappers::const_iterator EndD3DIt = m_Direct3DWrappers.end();
		Direct3DWrappers::const_iterator D3DIt = m_Direct3DWrappers.begin();
		DeviceWrappers::const_iterator EndDevIt = m_DeviceWrappers.end();
		DeviceWrappers::const_iterator DevIt = m_DeviceWrappers.begin();		

		for (; DevIt != EndDevIt; ++DevIt)
			delete *DevIt;

		m_DeviceWrappers.clear();

		for (; D3DIt != EndD3DIt; ++D3DIt)
			delete *D3DIt;

		m_Direct3DWrappers.clear();
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
				IDirect3D9Wrapper *pDirect3DWrapper = new IDirect3D9Wrapper(pDirect3D, m_VSync);
				IDirect3DDevice9Wrapper *pDeviceWrapper = NULL;

				m_Direct3DWrappers.push_back(pDirect3DWrapper);
				m_DeviceWrappers.push_back(pDeviceWrapper);				

				// subscribe for a pointer to the Direct3DDevice wrapper
				pDirect3DWrapper->Subscribe(&pDeviceWrapper);

				return pDirect3DWrapper;
			}
		}

		return NULL;
	}

	//! \brief Switches on/off the rendering added by the windower
	void GraphicsCore::ToggleRendering()
	{
		DeviceWrappers::const_iterator EndDevIt = m_DeviceWrappers.end();
		DeviceWrappers::const_iterator DevIt = m_DeviceWrappers.begin();

		for (; DevIt != EndDevIt; ++DevIt)
			(*DevIt)->ToggleRendering();
	}

	void GraphicsCore::SetRendering(bool bEnable_in)
	{
		DeviceWrappers::const_iterator EndDevIt = m_DeviceWrappers.end();
		DeviceWrappers::const_iterator DevIt = m_DeviceWrappers.begin();

		for (; DevIt != EndDevIt; ++DevIt)
			(*DevIt)->SetRendering(bEnable_in);
	}

	void GraphicsCore::ToggleWireframe()
	{
		DeviceWrappers::const_iterator EndDevIt = m_DeviceWrappers.end();
		DeviceWrappers::const_iterator DevIt = m_DeviceWrappers.begin();

		for (; DevIt != EndDevIt; ++DevIt)
			(*DevIt)->ToggleWireframe();
	}

	void GraphicsCore::ToggleFPS()
	{
		DeviceWrappers::const_iterator EndDevIt = m_DeviceWrappers.end();
		DeviceWrappers::const_iterator DevIt = m_DeviceWrappers.begin();

		for (; DevIt != EndDevIt; ++DevIt)
			(*DevIt)->ToggleFPS();
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