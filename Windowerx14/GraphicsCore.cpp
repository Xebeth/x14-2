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

#include "WindowerEngine.h"
#include "Direct3D9Hook.h"

#include "Font.h"
#include "IRenderable.h"
#include "IDirect3D9Wrapper.h"
#include "IDirect3DDevice9Wrapper.h"

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
	GraphicsCore::GraphicsCore(bool VSync_in)
		: WindowerCore(_T(GRAPHICS_MODULE)), m_VSync(VSync_in), m_pLabelRenderer(NULL),
		  m_pDirect3DWrapper(NULL), m_pDirect3DCreate9Trampoline(NULL),
		  m_SkipDeviceCount(1U), m_Width(0U), m_Height(0U), m_MouseOffsetX(0U),
		  m_MouseOffsetY(0U), m_pMovingLabel(NULL), m_pDeviceWrapper(NULL),
		  m_LabelHeight(0UL), m_LabelWidth (0UL) {}

	//! \brief GraphicsCore destructor
	GraphicsCore::~GraphicsCore()
	{
		RenderableMap::const_iterator RenderableIt = m_UiElements.cbegin();
		RenderableMap::const_iterator EndIt = m_UiElements.cend();

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

		if (m_pDeviceWrapper != NULL)
		{
			delete m_pDeviceWrapper;
			m_pDeviceWrapper = NULL;
		}

		if (m_pDirect3DWrapper != NULL)
		{
			delete m_pDirect3DWrapper;
			m_pDirect3DWrapper = NULL;
		}

		if (m_pDeviceWrapper != NULL)
		{
			delete m_pDeviceWrapper;
			m_pDeviceWrapper = NULL;
		}
	}

	bool GraphicsCore::RegisterServices()
	{
		// register the services
		return (RegisterService(_T(TEXT_LABEL_SERVICE), true) != NULL);
	}

	bool GraphicsCore::Invoke(const string_t& ServiceName_in, PluginFramework::ServiceParam &Params_in)
	{
		if (m_pDirect3DWrapper != NULL)
		{
			ModuleServices::const_iterator ServiceIt = m_Services.find(ServiceName_in);

			// the service exists and can be invoked
			if (ServiceIt != m_Services.cend() && ServiceIt->second->CanInvoke())
			{
				if (ServiceName_in.compare(_T(TEXT_LABEL_SERVICE)) == 0
					&& Params_in.DataType.compare(_T("LabelServiceParam")) == 0
					&& Params_in.pData != NULL)
				{
					TextLabelService *pService = static_cast<TextLabelService*>(ServiceIt->second);

					pService->SetRenderer(m_pDeviceWrapper, GetLabelRenderer());

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

	TextLabelRenderer* GraphicsCore::GetLabelRenderer()
	{
		if (m_pLabelRenderer == NULL)
			m_pLabelRenderer = new TextLabelRenderer(m_Width, m_Height);

		return m_pLabelRenderer;
	}


	void GraphicsCore::ToggleFPS()
	{
		RenderableMap::const_iterator RenderableIt = m_UiElements.find(GFX_TEXT_FPS);
		UiTextLabel *pFPSLabel = NULL;

		if (RenderableIt == m_UiElements.cend())
		{
			pFPSLabel = new UiFPSCounter(GFX_TEXT_FPS, m_pDeviceWrapper, _T("FPS##Label"), -10L, 24L, 60UL, 16UL,
										 _T("Arial"), 12, true, false, 0xFFFF0000, GetLabelRenderer(), true);

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

	void GraphicsCore::SetWindowSize(WORD Width_in, WORD Height_in)
	{
		m_Height = Height_in;
		m_Width = Width_in;

		if (m_pLabelRenderer != NULL)
			m_pLabelRenderer->SetWindowSize(Width_in, Height_in);
	}

	bool GraphicsCore::OnMouseMove(WORD X_in, WORD Y_in, DWORD MouseFlags_in)
	{
		if (m_pMovingLabel != NULL)
		{
			long PosX = m_MouseOffsetX + X_in;
			long PosY = m_MouseOffsetY + Y_in;

			if (PosX < 0L)
				PosX = 0L;
			else if (PosX + m_LabelWidth > m_Width)
				PosX = m_Width - m_LabelWidth;
			
			if (PosY < 0L)
				PosY = 0L;
			else if (PosY + m_LabelHeight > m_Height)
				PosY = m_Height - m_LabelHeight;

			m_pMovingLabel->SetPos(PosX, PosY);

			return true;
		}

		return false;
	}

	UiTextLabel* GraphicsCore::HitTest(WORD X_in, WORD Y_in)
	{
		RenderableMap::const_iterator RenderableIt = m_UiElements.cbegin();
		RenderableMap::const_iterator EndIt = m_UiElements.cend();
		UiTextLabel *pResult = NULL, *pLabel = NULL;

		for(; RenderableIt != EndIt; ++RenderableIt)
		{
			pLabel = static_cast<UiTextLabel*>(RenderableIt->second);

			if (pLabel != NULL && pLabel->IsVisible() && pLabel->HitTest(X_in, Y_in)
			 && (pResult == NULL || pLabel->GetZOrder() > pResult->GetZOrder()))
			{
				pResult = pLabel;
			}
		}

		return pResult;
	}

	bool GraphicsCore::OnLButtonUp(WORD X_in, WORD Y_in, DWORD MouseFlags_in)
	{
		bool Result = (m_pMovingLabel != NULL);

		m_pMovingLabel = NULL;

		return Result;
	}

	bool GraphicsCore::OnLButtonDown(WORD X_in, WORD Y_in, DWORD MouseFlags_in)
	{
		m_pMovingLabel = HitTest(X_in, Y_in);

		if (m_pMovingLabel != NULL)
		{
			long LabelX = 0UL, LabelY = 0UL;

			m_pMovingLabel->GetSize(m_LabelWidth, m_LabelHeight);
			m_pMovingLabel->GetScreenPos(LabelX, LabelY);			
			// get the offset between the position of the mouse and the label
			m_MouseOffsetX = LabelX - X_in;
			m_MouseOffsetY = LabelY - Y_in;

			return true;
		}

		return false;
	}

	bool GraphicsCore::IsMouseCaptured() const
	{
		return (m_pMovingLabel != NULL);
	}
}