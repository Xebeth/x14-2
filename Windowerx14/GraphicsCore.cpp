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

#include "WindowerSettings.h"
#include "WindowerEngine.h"

#include "Direct3D9Hook.h"

#include "Font.h"
#include "IRenderable.h"
#include "IDirect3D9Wrapper.h"
#include "IDirect3DDevice9Wrapper.h"
#include "Direct3D9Hook.h"

#include "TextLabelRenderer.h"
#include "UiTextLabel.h"
#include "UiFPSCounter.h"

#include "WindowerCore.h"
#include "GraphicsCore.h"

#include "ModuleService.h"
#include "TextLabelService.h"

using namespace UIAL;

namespace Windower
{
	/*! \brief GraphicsCore constructor
		\param[in,out] Engine_in_out : the windower engine
		\param[in] VSync_in : flag specifying if vertical synchronization is in use
	*/
	GraphicsCore::GraphicsCore(WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out, bool VSync_in)
		: WindowerCore(_T("Graphics"), Engine_in_out, HookManager_in_out), m_VSync(VSync_in), m_pLabelRenderer(NULL),
		  m_pDirect3DWrapper(NULL), m_pDirect3DCreate9Trampoline(NULL), m_SkipDeviceCount(1U) {}

	//! \brief GraphicsCore destructor
	GraphicsCore::~GraphicsCore()
	{
		RenderableMap::const_iterator RenderableIt = m_UiElements.cbegin();
		RenderableMap::const_iterator EndIt = m_UiElements.end();

		for(; RenderableIt != EndIt; ++RenderableIt)
		{
			if (m_pDeviceWrapper != NULL)
				m_pDeviceWrapper->RemoveRenderable(RenderableIt->first);

			delete RenderableIt->second;
		}

		m_UiElements.clear();

		if (m_pLabelRenderer != NULL)
		{
			delete m_pLabelRenderer;
			m_pLabelRenderer = NULL;
		}
	}

	bool GraphicsCore::RegisterServices()
	{
		// register the services
		return (RegisterService(_T("TextLabelCreation"), true) != NULL);
	}


	bool GraphicsCore::Invoke(const string_t& ServiceName_in, const PluginFramework::ServiceParam &Params_in)
	{
		if (m_pDirect3DWrapper != NULL)
		{
			ModuleServices::iterator Iter = m_Services.find(ServiceName_in);

			// the service exists and can be invoked
			if (Iter != m_Services.end() && Iter->second->CanInvoke())
			{
				if (ServiceName_in.compare(_T("TextLabelCreation")) == 0
					&& Params_in.DataType.compare(_T("LabelCreationParam")) == 0
					&& Params_in.pData != NULL)
				{
					TextLabelService *pService = static_cast<TextLabelService*>(Iter->second);					

					pService->SetRenderer(m_pDeviceWrapper, m_pLabelRenderer);

					return pService->Invoke(Params_in);
				}
			}
		}

		return false;
	}

	/*! \brief Creates a Direct3D device given a DirectX SDK version
		\param[in] SDKVersion_in : the DirectX SDK version
		\return a pointer to the new device
	*/
	IDirect3D9* GraphicsCore::Direct3DCreate9Hook(UINT SDKVersion_in)
	{
		IDirect3D9* pDirect3D = NULL;

		if (m_pDirect3DCreate9Trampoline != NULL)
		{
			pDirect3D = m_pDirect3DCreate9Trampoline(SDKVersion_in);

			if (pDirect3D != NULL && m_SkipDeviceCount == 0)
			{
				IDirect3D9Wrapper *pDirect3DWrapper = new IDirect3D9Wrapper(pDirect3D, m_VSync);

				m_pDirect3DWrapper = pDirect3DWrapper;
				// subscribe for a pointer to the Direct3DDevice wrapper
				pDirect3DWrapper->Subscribe(&m_pDeviceWrapper);

				return pDirect3DWrapper;
			}
			else
				--m_SkipDeviceCount;
		}

		return pDirect3D;
	}

	//! \brief Switches on/off the rendering added by the windower
	void GraphicsCore::ToggleRendering()
	{
		if (m_pDeviceWrapper != NULL)
			m_pDeviceWrapper->ToggleRendering();
	}

	void GraphicsCore::SetRendering(bool bEnable_in)
	{
		if (m_pDeviceWrapper != NULL)
			m_pDeviceWrapper->SetRendering(bEnable_in);
	}

	void GraphicsCore::ToggleFPS()
	{
		if (m_pLabelRenderer == NULL)
			m_pLabelRenderer = new TextLabelRenderer(m_pDeviceWrapper);

		RenderableMap::const_iterator RenderableIt = m_UiElements.find(GFX_TEXT_FPS);
		UiTextLabel *pFPSLabel = NULL;

		if (RenderableIt == m_UiElements.end())
		{
			pFPSLabel = new UiFPSCounter(GFX_TEXT_FPS, m_pDeviceWrapper, _T("FPS##Label"), -10L, 24L, 60UL, 20UL,
										 _T("Arial"), 12, true, false, 0xFFFF0000, m_pLabelRenderer, true);

			m_pDeviceWrapper->AddRenderable(GFX_TEXT_FPS, pFPSLabel);
			m_UiElements[GFX_TEXT_FPS] = pFPSLabel;
		}
		else
		{
			pFPSLabel = static_cast<UiTextLabel*>(RenderableIt->second);

			if (pFPSLabel != NULL && pFPSLabel->ToggleVisible())
				pFPSLabel->SetPos(-10L, 24L);
		}
	}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void GraphicsCore::RegisterHooks(HookEngineLib::IHookManager &HookManager_in)
	{
		// register the Direct3DCreate9 hook
		HookManager_in.RegisterHook("Direct3DCreate9", "d3d9.dll", NULL, ::Direct3DCreate9Hook);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void GraphicsCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pDirect3DCreate9Trampoline = (fnDirect3DCreate9)HookManager_in.GetTrampolineFunc("Direct3DCreate9");		
	}

	/*! \brief Creates a service object given its name
		\param[in] ServiceName_in : the name of the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
		\return a pointer to the service object if successful; NULL otherwise
	*/
	BaseModuleService* GraphicsCore::CreateService(const string_t& ServiceName_in, bool InvokePermission_in)
	{
		return new TextLabelService(ServiceName_in, m_UiElements, GFX_TEXT_COUNT, InvokePermission_in);
	}
}