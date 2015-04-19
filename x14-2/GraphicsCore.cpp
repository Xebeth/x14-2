/**************************************************************************
	created		:	2010-10-10
	filename	: 	GraphicsCore.cpp
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interface with Direct3D 9
**************************************************************************/
#include "stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>

#include "WindowerEngine.h"

#include "Font.h"
#include "IRenderable.h"
#include "IEventInterface.h"
#include "IDirect3D9Wrapper.h"
#include "Direct3D9WrapperImpl.h"
#include "IDeviceCreateSubscriber.h"
#include "IDirect3DDevice9Wrapper.h"
#include "Direct3DDevice9WrapperImpl.h"
#include "Direct3DSwapChain9WrapperImpl.h"

#include "TextLabelRenderer.h"
#include "UiTextLabel.h"
#include "UiFPSCounter.h"

#include "WindowerCore.h"
#include "GraphicsCore.h"

#include "ModuleService.h"
#include "TextLabelService.h"

extern Windower::WindowerEngine *g_pEngine;

using namespace UIAL;

Direct3DSwapChain9WrapperImpl *g_pDirect3DSwapChainWrapper = NULL;
Direct3DDevice9WrapperImpl *g_pDeviceWrapperImpl = NULL;
Direct3D9WrapperImpl *g_pDirect3DWrapper = NULL;

namespace Windower
{
	static void WINAPI OnMacroLabelClick(UIAL::CUiWindow<> *pLabel, WORD X_in, WORD Y_in, DWORD MouseFlags_in, UINT KeyFlags_in)
	{
		if (g_pEngine != NULL && (KeyFlags_in & IEventInterface::CONTROL_KEY_FLAG) == IEventInterface::CONTROL_KEY_FLAG)
			g_pEngine->AbortMacro();
	}

	WindowerCore::CallingContext<GraphicsCore> GraphicsCore::m_Context;
	
	/*! \brief GraphicsCore constructor
		\param[in,out] Engine_in_out : the windower engine
		\param[in] VSync_in : flag specifying if vertical synchronization is in use
	*/
	GraphicsCore::GraphicsCore(bool VSync_in)
		: WindowerCore(_T(GRAPHICS_MODULE)), m_pLabelRenderer(NULL),
		  m_pDirect3DCreate9Trampoline(NULL), m_Width(0U), m_Height(0U),
		  m_pMovingLabel(NULL), m_pDirect3DDevice(NULL),
		  m_MouseOffsetX(0U), m_MouseOffsetY(0U),
		  m_LabelWidth (0UL), m_LabelHeight(0UL)
	{
		// set the calling context for the hooks
		m_Context.Set(this);
	}

	//! \brief GraphicsCore destructor
	GraphicsCore::~GraphicsCore()
	{
		RenderableMap::const_iterator RenderableIt = m_UiElements.cbegin();
		RenderableMap::const_iterator EndIt = m_UiElements.cend();

		for(; RenderableIt != EndIt; ++RenderableIt)
		{
			if (g_pDeviceWrapperImpl != NULL)
				g_pDeviceWrapperImpl->RemoveRenderable(RenderableIt->first);

			delete RenderableIt->second;
		}

		m_UiElements.clear();

		if (m_pLabelRenderer != NULL)
		{
			delete m_pLabelRenderer;
			m_pLabelRenderer = NULL;
		}

		Detach();
	}

	void GraphicsCore::Detach()
	{
		if (g_pDirect3DWrapper != NULL)
		{
			delete g_pDirect3DWrapper;
			g_pDirect3DWrapper = NULL;
		}

		if (g_pDirect3DSwapChainWrapper != NULL)
		{
			delete g_pDirect3DSwapChainWrapper;
			g_pDirect3DSwapChainWrapper = NULL;
		}

		if (g_pDeviceWrapperImpl != NULL)
		{
			g_pDeviceWrapperImpl->SetRendering(false);
			delete g_pDeviceWrapperImpl;
			g_pDeviceWrapperImpl = NULL;
		}
	}

	bool GraphicsCore::RegisterServices()
	{
		// register the services
		return (RegisterService(_T(TEXT_LABEL_SERVICE), true) != NULL);
	}

	bool GraphicsCore::Invoke(const string_t& ServiceName_in, PluginFramework::ServiceParam &Params_in)
	{
		if (g_pDeviceWrapperImpl != NULL)
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

					pService->SetRenderer(GetLabelRenderer(), m_pDirect3DDevice, g_pDeviceWrapperImpl);

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

		if (m_Context->m_pDirect3DCreate9Trampoline == NULL)
			m_Context->m_pDirect3DCreate9Trampoline = (fnDirect3DCreate9)m_pHookManager->GetTrampolineFunc("Direct3DCreate9");

		if (m_Context->m_pDirect3DCreate9Trampoline != NULL)
		{
			static DWORD CallCount = 1UL;

			pDirect3D = m_Context->m_pDirect3DCreate9Trampoline(SDKVersion_in);

			if (pDirect3D != NULL && CallCount == 0UL)
			{
				// create the wrapper implementation
				g_pDirect3DWrapper = new Direct3D9WrapperImpl(pDirect3D, m_Context->m_VSync, m_Context);

				// remove the hook
				if (m_pHookManager != NULL)
					m_pHookManager->UnregisterHook("Direct3DCreate9");

				return pDirect3D;
			}
			else
				--CallCount;
		}
		else
		{
			pDirect3D = Direct3DCreate9(SDKVersion_in);
		}

		return pDirect3D;
	}

	void GraphicsCore::OnDeviceCreate(IDirect3DDevice9 *pDevice_in, const D3DPRESENT_PARAMETERS *pPresentParams_in, HWND hWnd_in)
	{
		m_pDirect3DDevice = pDevice_in;
		m_hWnd = hWnd_in;

		// set the device implementation
		if (pDevice_in != NULL && g_pDeviceWrapperImpl == NULL && pPresentParams_in != NULL)
		{
			D3DPRESENT_PARAMETERS PresentParams = *pPresentParams_in;

			if (m_Context->m_VSync)
			{
				// only these matter, they are used to overwrite
				// the parameters during IDirect3DDevice9::Reset()
				PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
				PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
				PresentParams.BackBufferCount = 2;
			}

			TextLabelRenderer *pRenderer = GetLabelRenderer();
			// create the FPS counter and macro progress
			UiTextLabel *pFpsLabel = new UiFPSCounter(GFX_TEXT_FPS, m_pDirect3DDevice, FPS_LABEL_NAME, -16L, 0L, 60UL, 16UL,
													  _T("Arial"), 12U, true, false, 0xFFFF0000, pRenderer, true);
			UiTextLabel *pMacroLabel = new UiTimerCounter(GFX_TEXT_MACRO, m_pDirect3DDevice, MACRO_LABEL_NAME, 0L, 0L, 320UL, 16UL,
														  _T("Arial"), 12U, true, false, 0xFFFF0000, pRenderer, true);
			// draw once to force w/e ID3DXFont::DrawText does the first time
			// (one of the side effect is resetting the vtable of the device)
			pFpsLabel->Draw();
			// create the device wrapper implementation
			g_pDeviceWrapperImpl = new Direct3DDevice9WrapperImpl(pDevice_in, PresentParams);
			// initialize the labels
			InitializeLabel(pMacroLabel);
			InitializeLabel(pFpsLabel);

			pMacroLabel->SetMouseClickEvent(OnMacroLabelClick);
#ifdef _DEBUG
			pFpsLabel->SetVisibile(true);
#endif // _DEBUG
		}
		// try to restore Direct3D
		if (g_pDirect3DWrapper != NULL)
		{
			delete g_pDirect3DWrapper;
			g_pDirect3DWrapper = NULL;
		}
	}

	//! \brief Switches on/off the rendering added by the windower
	void GraphicsCore::ToggleRendering()
	{
		if (g_pDeviceWrapperImpl != NULL)
			g_pDeviceWrapperImpl->ToggleRendering();
	}

	void GraphicsCore::SetRendering(bool bEnable_in)
	{
		if (g_pDeviceWrapperImpl != NULL)
			g_pDeviceWrapperImpl->SetRendering(bEnable_in);
	}

	TextLabelRenderer* GraphicsCore::GetLabelRenderer()
	{
		if (m_pLabelRenderer == NULL)
			m_pLabelRenderer = new TextLabelRenderer(m_Width, m_Height);

		return m_pLabelRenderer;
	}

	void GraphicsCore::ToggleFPS()
	{
		if (g_pDeviceWrapperImpl != NULL)
		{
			RenderableMap::const_iterator RenderableIt = m_UiElements.find(GFX_TEXT_FPS);

			if (RenderableIt != m_UiElements.cend())
			{
				UiTextLabel *pFPSLabel = static_cast<UiTextLabel*>(RenderableIt->second);

				if (pFPSLabel != NULL && pFPSLabel->ToggleVisible())
				{
					unsigned long TextColor = 0xFFFF0000;
					bool Bold = true, Italic = false;
					string_t FontName = _T("Arial");
					unsigned short FontSize = 12U;
					long X = -16L, Y = 0L;

					m_pEngine->DeserializeLabel(pFPSLabel->GetName(), X, Y, TextColor, FontName, FontSize, Bold, Italic);
					pFPSLabel->SetTitleFont(FontName, FontSize, Bold, Italic);
					pFPSLabel->SetTitleColor(TextColor);
					pFPSLabel->SetPos(X, Y);
				}					
			}
		}
	}

	void GraphicsCore::ShowMacroProgress(unsigned long current, unsigned long total, bool visible)
	{
		if (g_pDeviceWrapperImpl != NULL)
		{
			RenderableMap::const_iterator RenderableIt = m_UiElements.find(GFX_TEXT_MACRO);

			if (RenderableIt != m_UiElements.cend())
			{
				UiTimerCounter *pMacroLabel = static_cast<UiTimerCounter*>(RenderableIt->second);

				if (pMacroLabel != NULL)
				{
					unsigned long TextColor = 0xFFFF0000;
					bool Bold = true, Italic = false;
					string_t FontName = _T("Arial");
					unsigned short FontSize = 12U;
					long X = -16L, Y = 0L;
					
					m_pEngine->DeserializeLabel(pMacroLabel->GetName(), X, Y, TextColor, FontName, FontSize, Bold, Italic);
					pMacroLabel->SetTitleFont(FontName, FontSize, Bold, Italic);
					pMacroLabel->SetProgress(current, total, !visible);
					pMacroLabel->SetTitleColor(TextColor);
					pMacroLabel->SetVisible(visible);
					pMacroLabel->SetPos(X, Y);
				}
			}
		}
	}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void GraphicsCore::RegisterHooks(HookEngineLib::IHookManager &HookManager_in)
	{
		// register the Direct3DCreate9 hook
		HookManager_in.RegisterHook("Direct3DCreate9", "d3d9.dll", NULL, &GraphicsCore::Direct3DCreate9Hook);
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
		return new TextLabelService(*m_pEngine, ServiceName_in, m_UiElements, GFX_TEXT_COUNT, InvokePermission_in);
	}

	LRESULT GraphicsCore::OnSize(int NewWidth_in, int NewHeight_in, UINT nFlags_in)
	{
		m_Height = NewHeight_in;
		m_Width = NewWidth_in;

		if (m_pLabelRenderer != NULL)
			m_pLabelRenderer->SetWindowSize(NewWidth_in, NewHeight_in);

		return IEventInterface::EVENT_IGNORED;
	}

	LRESULT GraphicsCore::OnMouseMove(WORD X_in, WORD Y_in, DWORD MouseFlags_in, UINT KeyFlags_in)
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

			return IEventInterface::EVENT_PROCESSED;
		}

		return IEventInterface::EVENT_IGNORED;
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

	LRESULT GraphicsCore::OnLButtonUp(WORD X_in, WORD Y_in, DWORD MouseFlags_in, UINT KeyFlags_in)
	{
		if (m_pMovingLabel != NULL)
		{
			const UIAL::CUiFont &Font = m_pMovingLabel->GetTitleFont();

			m_pMovingLabel->OnMouseClick(X_in, Y_in, MouseFlags_in, KeyFlags_in);
			m_pEngine->SerializeLabel(m_pMovingLabel->GetName(),
									  m_pMovingLabel->GetX(), m_pMovingLabel->GetY(),
									  m_pMovingLabel->GetTitleColor().GetARGB(),
									  Font.GetFontName(), Font.GetFontSize(),
									  Font.IsBold(), Font.IsItalic());
			m_pMovingLabel = NULL;

			return IEventInterface::EVENT_PROCESSED;
		}

		return IEventInterface::EVENT_IGNORED;
	}

	LRESULT GraphicsCore::OnLButtonDown(WORD X_in, WORD Y_in, DWORD MouseFlags_in, UINT KeyFlags_in)
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

			return IEventInterface::EVENT_PROCESSED;
		}

		return IEventInterface::EVENT_IGNORED;
	}

	LRESULT GraphicsCore::OnKeyUp(UINT PressedChar_in, UINT RepetitionCount_in, UINT KeyFlags_in)
	{
		switch (PressedChar_in)
		{
			case VK_F11:
				if (KeyFlags_in == 0)
					::ShowWindow(m_hWnd, SW_MINIMIZE);
				else
					ToggleRendering();
				
				return IEventInterface::EVENT_PROCESSED; // filtered
			break;
			case VK_F12:
				ToggleFPS();
				
				return IEventInterface::EVENT_PROCESSED; // filtered
			break;
		}

		return IEventInterface::EVENT_IGNORED;
	}

	LRESULT GraphicsCore::OnActivate(bool bActive_in, bool bMinimized_in)
	{
		if (g_pDeviceWrapperImpl != NULL && (g_pDeviceWrapperImpl->IsFullscreen() || bMinimized_in))
		{
			g_pDeviceWrapperImpl->SetRendering(bActive_in);

			return IEventInterface::EVENT_PROCESSED; // filtered
		}

		return IEventInterface::EVENT_IGNORED;
	}

	void GraphicsCore::InitializeLabel(UiTextLabel *pLabel)
	{
		if (pLabel != NULL && g_pDeviceWrapperImpl != NULL)
		{
			unsigned long TextColor = 0xFFFF0000, ID = pLabel->GetID();
			const string_t &name = pLabel->GetName();
			bool Bold = true, Italic = false;
			string_t FontName = _T("Arial");
			unsigned short FontSize = 12U;
			long X = -16L, Y = 0L;

			bool Deserialized = m_pEngine->DeserializeLabel(name, X, Y, TextColor, FontName, FontSize, Bold, Italic);

			if (Deserialized == false)
				m_pEngine->SerializeLabel(name, X, Y, TextColor, FontName, FontSize);

			pLabel->SetVisibile(false);

			// add it to the list of renderables
			g_pDeviceWrapperImpl->AddRenderable(ID, pLabel);
			m_UiElements[ID] = pLabel;
		}
	}
}