/**************************************************************************
	created		:	2010-10-10
	filename	: 	GraphicsCore.cpp
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interface with Direct3D 9
**************************************************************************/
#include "stdafx.h"
#include <d3d11.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "WindowerEngine.h"

#include "IRenderable.h"
#include "IEventInterface.h"

#include "Font.h"
#include "IDirect3D9Wrapper.h"
#include "Direct3D9WrapperImpl.h"
#include "IDeviceCreateSubscriber.h"
#include "IDirect3DDevice9Wrapper.h"
#include "Direct3DDevice9WrapperImpl.h"
#include "Direct3DSwapChain9WrapperImpl.h"

#include "ISwapChainCreateSubscriber.h"
#include "DXGISwapChainWrapperImpl.h"
#include "DXGIFactoryWrapperImpl.h"

#include "TextLabelRenderer.h"
#include "UiTextLabel.h"
#include "UiFPSCounter.h"

#include "WindowerCore.h"
#include "GraphicsCore.h"

#include "ModuleService.h"
#include "TextLabelService.h"

DXGISwapChainWrapperImpl *g_pDXGISwapChainWrapper = nullptr;
DXGIFactoryWrapperImpl *g_pDXGIFactoryWrapper = nullptr;

Direct3DSwapChain9WrapperImpl *g_pDirect3DSwapChainWrapper = nullptr;
Direct3DDevice9WrapperImpl *g_pDeviceWrapperImpl = nullptr;
Direct3D9WrapperImpl *g_pDirect3DWrapper = nullptr;

extern Windower::WindowerEngine *g_pEngine;

namespace Windower
{
	static void WINAPI OnMacroLabelClick(UIAL::CUiWindow<> *pLabel, WORD X_in, WORD Y_in, DWORD_PTR MouseFlags_in, UINT_PTR KeyFlags_in)
	{
		if (g_pEngine != nullptr && (KeyFlags_in & IEventInterface::CONTROL_KEY_FLAG) == IEventInterface::CONTROL_KEY_FLAG)
			g_pEngine->AbortMacro();
	}

	WindowerCore::CallingContext<GraphicsCore> GraphicsCore::m_Context;
	
	/*! \brief GraphicsCore constructor
		\param[in,out] Engine_in_out : the windower engine
		\param[in] VSync_in : flag specifying if vertical synchronization is in use
	*/
	GraphicsCore::GraphicsCore(bool VSync_in)
		: WindowerCore(_T(GRAPHICS_MODULE)), m_pLabelRenderer(nullptr),
		  m_Width(0U), m_Height(0U), m_pMovingLabel(nullptr),
		  m_MouseOffsetX(0U), m_MouseOffsetY(0U),
		  m_LabelWidth (0UL), m_LabelHeight(0UL),
		  m_pD3D11CreateDeviceTrampoline(nullptr),
		  m_pCreateDXGIFactoryTrampoline(nullptr),
		  m_pDirect3DCreate9Trampoline(nullptr), m_pCreateSwapChainTrampoline(nullptr),
		  m_pDirect3DDevice(nullptr), m_pD3D11Device(nullptr)
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
			if (g_pDeviceWrapperImpl != nullptr)
				g_pDeviceWrapperImpl->RemoveRenderable(RenderableIt->first);

			delete RenderableIt->second;
		}

		m_UiElements.clear();

		if (m_pLabelRenderer != nullptr)
		{
			delete m_pLabelRenderer;
			m_pLabelRenderer = nullptr;
		}

		Detach();
	}

	void GraphicsCore::Detach()
	{
		if (g_pDirect3DWrapper != nullptr)
		{
			delete g_pDirect3DWrapper;
			g_pDirect3DWrapper = nullptr;
		}

		if (g_pDirect3DSwapChainWrapper != nullptr)
		{
			delete g_pDirect3DSwapChainWrapper;
			g_pDirect3DSwapChainWrapper = nullptr;
		}

		if (g_pDeviceWrapperImpl != nullptr)
		{
			g_pDeviceWrapperImpl->SetRendering(false);
			delete g_pDeviceWrapperImpl;
			g_pDeviceWrapperImpl = nullptr;
		}

		if (g_pDXGISwapChainWrapper != nullptr)
		{
			g_pDXGISwapChainWrapper->SetRendering(false);
			delete g_pDXGISwapChainWrapper;
			g_pDXGISwapChainWrapper = nullptr;
		}

		if (g_pDXGIFactoryWrapper != nullptr)
		{
			delete g_pDXGIFactoryWrapper;
			g_pDXGIFactoryWrapper = nullptr;
		}
	}

	bool GraphicsCore::RegisterServices()
	{
		// register the services
		return (RegisterService(_T(TEXT_LABEL_SERVICE), true) != nullptr);
	}

	bool GraphicsCore::Invoke(const string_t& ServiceName_in, PluginFramework::ServiceParam &Params_in)
	{
		if (g_pDeviceWrapperImpl != nullptr)
		{
			ModuleServices::const_iterator ServiceIt = m_Services.find(ServiceName_in);

			// the service exists and can be invoked
			if (ServiceIt != m_Services.cend() && ServiceIt->second->CanInvoke())
			{
				if (ServiceName_in.compare(_T(TEXT_LABEL_SERVICE)) == 0
					&& Params_in.DataType.compare(_T("LabelServiceParam")) == 0
					&& Params_in.pData != nullptr)
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
		IDirect3D9* pDirect3D = nullptr;

		if (m_Context->m_pDirect3DCreate9Trampoline == nullptr)
			m_Context->m_pDirect3DCreate9Trampoline = (fnDirect3DCreate9)m_pHookManager->GetTrampolineFunc("Direct3DCreate9");

		if (m_Context->m_pDirect3DCreate9Trampoline != nullptr)
		{
			static DWORD CallCount = 1UL;

			pDirect3D = m_Context->m_pDirect3DCreate9Trampoline(SDKVersion_in);

			if (pDirect3D != nullptr && CallCount == 0UL)
			{
				// create the wrapper implementation
				g_pDirect3DWrapper = new Direct3D9WrapperImpl(pDirect3D, m_Context->m_VSync, m_Context);

				// remove the hook
				if (m_pHookManager != nullptr)
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

	HRESULT WINAPI GraphicsCore::CreateDXGIFactoryHook(REFIID riid, void **ppFactory)
	{
		HRESULT result = m_Context->m_pCreateDXGIFactoryTrampoline(riid, ppFactory);

		if (SUCCEEDED(result))
		{
			string_t debug;

			format(debug, L"CreateDXGIFactoryHook: %p %ul-%u-%u-%S\n", *ppFactory,
				   riid.Data1, riid.Data2, riid.Data3, riid.Data4);

			OutputDebugString(debug.c_str());

			if (g_pDXGIFactoryWrapper == nullptr)


			if (ppFactory != nullptr && g_pDXGIFactoryWrapper != nullptr)
				m_Context->OnDXGIFactoryCreate(*(IDXGIFactory**)ppFactory);
		}

		m_Context->m_pHookManager->UnregisterHook("CreateDXGIFactory");

		return result;
	} 

	HRESULT WINAPI GraphicsCore::D3D11CreateDeviceHook(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software,
													   UINT Flags, CONST D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels,
													   UINT SDKVersion, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel,
													   ID3D11DeviceContext** ppImmediateContext)
	{
		m_Context->m_pHookManager->RegisterHook("CreateDXGIFactory", "dxgi.dll", nullptr, &GraphicsCore::CreateDXGIFactoryHook);
		m_Context->m_pCreateDXGIFactoryTrampoline = (fnCreateDXGIFactory)m_Context->m_pHookManager->InstallHook("CreateDXGIFactory");

		HRESULT result = m_Context->m_pD3D11CreateDeviceTrampoline(pAdapter, DriverType, Software, Flags,
																			   pFeatureLevels, FeatureLevels, SDKVersion,
																			   ppDevice, pFeatureLevel, ppImmediateContext);
		if (SUCCEEDED(result) && ppDevice != nullptr)
			m_Context->m_pD3D11Device = *ppDevice;

		m_Context->m_pHookManager->UnregisterHook("D3D11CreateDevice");

		return result;
	}



	void GraphicsCore::OnDXGIFactoryCreate(IDXGIFactory *pFactory_in)
	{
		if (pFactory_in != nullptr)
		{
			DXGIFactory_vTable::Entries *pvTable = *(DXGIFactory_vTable::Entries**)pFactory_in;

			if (m_pHookManager->BeginTransaction())
			{
				m_pHookManager->RegisterHook("CreateSwapChain", SIGSCAN_GAME_PROCESSA, *(LPVOID*)&pvTable->pfnCreateSwapChain, (LPVOID)&GraphicsCore::CreateSwapChainHook);
				m_pCreateSwapChainTrampoline = (fnCreateSwapChain)m_pHookManager->InstallHook("CreateSwapChain");
				m_pHookManager->CommitTransaction();
			}
		}
	}

	void GraphicsCore::OnDeviceCreate(IDirect3DDevice9 *pDevice_in, const D3DPRESENT_PARAMETERS *pPresentParams_in, HWND hWnd_in)
	{
		m_pDirect3DDevice = pDevice_in;
		m_hWnd = hWnd_in;

		// set the device implementation
		if (pDevice_in != nullptr && g_pDeviceWrapperImpl == nullptr && pPresentParams_in != nullptr)
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
			// create the device wrapper implementation
			g_pDeviceWrapperImpl = new Direct3DDevice9WrapperImpl(pDevice_in, PresentParams);
			// initialize labels and their renderer
			InitializeRenderer(pDevice_in);
		}
		// try to restore Direct3D
		if (g_pDirect3DWrapper != nullptr)
		{
			delete g_pDirect3DWrapper;
			g_pDirect3DWrapper = nullptr;
		}
	}

	void GraphicsCore::OnSwapChainCreate(IDXGISwapChain *pSwapChain, const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc)
	{
		if (g_pDXGISwapChainWrapper != nullptr)
		{
			g_pDXGISwapChainWrapper->SetRendering(false);
			delete g_pDXGISwapChainWrapper;
			g_pDXGISwapChainWrapper = nullptr;
		}

		if (pSwapChain != nullptr)
			g_pDXGISwapChainWrapper = new DXGISwapChainWrapperImpl(pSwapChain, pSwapChainDesc);
	}

	void GraphicsCore::InitializeRenderer(IDirect3DDevice9 *pDevice_in)
	{
		if (pDevice_in != nullptr)
		{
			TextLabelRenderer *pRenderer = GetLabelRenderer();
			// create the FPS counter and macro progress
			UiTextLabel *pFpsLabel = new UiFPSCounter(GFX_TEXT_FPS, pDevice_in, FPS_LABEL_NAME, -16L, 0L, 60UL, 16UL,
													  _T("Arial"), 12U, true, false, 0xFFFF0000, pRenderer, true);
			UiTextLabel *pMacroLabel = new UiTimerCounter(GFX_TEXT_MACRO, pDevice_in, MACRO_LABEL_NAME, 0L, 0L, 320UL, 16UL,
														  _T("Arial"), 12U, true, false, 0xFFFF0000, pRenderer, true);
			// draw once to force w/e ID3DXFont::DrawText does the first time
			// (one of the side effect is resetting the vtable of the device)
			pFpsLabel->Draw();
			// initialize the labels
			InitializeLabel(pMacroLabel);
			InitializeLabel(pFpsLabel);

			pMacroLabel->SetMouseClickEvent(OnMacroLabelClick);
#ifdef _DEBUG
			pFpsLabel->SetVisibile(true);
#endif // _DEBUG
		}
	}

	//! \brief Switches on/off the rendering added by the windower
	void GraphicsCore::ToggleRendering()
	{
		if (g_pDeviceWrapperImpl != nullptr)
			g_pDeviceWrapperImpl->ToggleRendering();
	}

	void GraphicsCore::SetRendering(bool bEnable_in)
	{
		if (g_pDeviceWrapperImpl != nullptr)
			g_pDeviceWrapperImpl->SetRendering(bEnable_in);
	}

	TextLabelRenderer* GraphicsCore::GetLabelRenderer()
	{
		if (m_pLabelRenderer == nullptr)
			m_pLabelRenderer = new TextLabelRenderer(m_Width, m_Height);

		return m_pLabelRenderer;
	}

	void GraphicsCore::ToggleFPS()
	{
		if (g_pDeviceWrapperImpl != nullptr)
		{
			RenderableMap::const_iterator RenderableIt = m_UiElements.find(GFX_TEXT_FPS);

			if (RenderableIt != m_UiElements.cend())
			{
				UiTextLabel *pFPSLabel = static_cast<UiTextLabel*>(RenderableIt->second);

				if (pFPSLabel != nullptr && pFPSLabel->ToggleVisible())
				{
					bool Bold = true, Italic = false, Collapsed = false;
					unsigned long TextColor = 0xFFFF0000;					
					string_t FontName = _T("Arial");
					unsigned short FontSize = 12U;
					long X = -16L, Y = 0L;

					m_pEngine->DeserializeLabel(pFPSLabel->GetName(), X, Y, TextColor, FontName, FontSize, Bold, Italic, Collapsed);
					pFPSLabel->SetTitleFont(FontName, FontSize, Bold, Italic);
					pFPSLabel->SetTitleColor(TextColor);
					pFPSLabel->SetPos(X, Y);
				}					
			}
		}
	}

	void GraphicsCore::ShowMacroProgress(unsigned long current, unsigned long total, bool visible)
	{
		if (g_pDeviceWrapperImpl != nullptr)
		{
			RenderableMap::const_iterator RenderableIt = m_UiElements.find(GFX_TEXT_MACRO);

			if (RenderableIt != m_UiElements.cend())
			{
				UiTimerCounter *pMacroLabel = static_cast<UiTimerCounter*>(RenderableIt->second);

				if (pMacroLabel != nullptr)
				{
					bool Bold = true, Italic = false, Collapsed = false;
					unsigned long TextColor = 0xFFFF0000;
					string_t FontName = _T("Arial");
					unsigned short FontSize = 12U;
					long X = -16L, Y = 0L;
					
					m_pEngine->DeserializeLabel(pMacroLabel->GetName(), X, Y, TextColor, FontName, FontSize, Bold, Italic, Collapsed);
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
		HookManager_in.RegisterHook("Direct3DCreate9", "d3d9.dll", nullptr, &GraphicsCore::Direct3DCreate9Hook);
		HookManager_in.RegisterHook("D3D11CreateDevice", "d3d11.dll", nullptr, &GraphicsCore::D3D11CreateDeviceHook);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void GraphicsCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pDirect3DCreate9Trampoline = (fnDirect3DCreate9)HookManager_in.GetTrampolineFunc("Direct3DCreate9");
		m_pD3D11CreateDeviceTrampoline = (PFN_D3D11_CREATE_DEVICE)HookManager_in.GetTrampolineFunc("D3D11CreateDevice");
	}

	HRESULT WINAPI GraphicsCore::CreateSwapChainHook(IDXGIFactory *pFactory, IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain)
	{
		HRESULT result = E_NOTIMPL;

		if (m_Context->m_pCreateSwapChainTrampoline != nullptr)
		{
			result = m_Context->m_pCreateSwapChainTrampoline(pFactory, pDevice, pDesc, ppSwapChain);
			
			if (SUCCEEDED(result) && ppSwapChain != nullptr)
				m_Context->OnSwapChainCreate(*ppSwapChain, pDesc);
		}

		return result;
	}

	/*! \brief Creates a service object given its name
		\param[in] ServiceName_in : the name of the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
		\return a pointer to the service object if successful; nullptr otherwise
	*/
	BaseModuleService* GraphicsCore::CreateService(const string_t& ServiceName_in, bool InvokePermission_in)
	{
		return new TextLabelService(*m_pEngine, ServiceName_in, m_UiElements, GFX_TEXT_COUNT, InvokePermission_in);
	}

	LRESULT GraphicsCore::OnSize(int NewWidth_in, int NewHeight_in, UINT nFlags_in)
	{
		m_Height = NewHeight_in;
		m_Width = NewWidth_in;

		if (m_pLabelRenderer != nullptr)
			m_pLabelRenderer->SetWindowSize(NewWidth_in, NewHeight_in);

		return IEventInterface::EVENT_IGNORED;
	}

	LRESULT GraphicsCore::OnMouseMove(WORD X_in, WORD Y_in, DWORD_PTR MouseFlags_in, UINT_PTR KeyFlags_in)
	{
		if (m_pMovingLabel != nullptr)
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
		UiTextLabel *pResult = nullptr, *pLabel = nullptr;

		for(; RenderableIt != EndIt; ++RenderableIt)
		{
			pLabel = static_cast<UiTextLabel*>(RenderableIt->second);

			if (pLabel != nullptr && pLabel->IsVisible() && pLabel->HitTest(X_in, Y_in)
			 && (pResult == nullptr || pLabel->GetZOrder() > pResult->GetZOrder()))
			{
				pResult = pLabel;
			}
		}

		return pResult;
	}

	LRESULT GraphicsCore::OnLButtonUp(WORD X_in, WORD Y_in, DWORD_PTR MouseFlags_in, UINT_PTR KeyFlags_in)
	{
		if (m_pMovingLabel != nullptr)
		{
			const UIAL::CUiFont &Font = m_pMovingLabel->GetTitleFont();

			m_pMovingLabel->OnMouseClick(X_in, Y_in, MouseFlags_in, KeyFlags_in);
			m_pEngine->SerializeLabel(m_pMovingLabel->GetName(),
									  m_pMovingLabel->GetX(), m_pMovingLabel->GetY(),
									  m_pMovingLabel->GetTitleColor().GetARGB(),
									  Font.GetFontName(), Font.GetFontSize(),
									  Font.IsBold(), Font.IsItalic(),
									  m_pMovingLabel->IsCollapsed());
			m_pMovingLabel = nullptr;

			return IEventInterface::EVENT_PROCESSED;
		}

		return IEventInterface::EVENT_IGNORED;
	}

	LRESULT GraphicsCore::OnLButtonDown(WORD X_in, WORD Y_in, DWORD_PTR MouseFlags_in, UINT_PTR KeyFlags_in)
	{
		m_pMovingLabel = HitTest(X_in, Y_in);

		if (m_pMovingLabel != nullptr)
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

	LRESULT GraphicsCore::OnKeyUp(UINT_PTR PressedChar_in, UINT RepetitionCount_in, UINT_PTR KeyFlags_in)
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
				if (KeyFlags_in == 0)
				{
					ToggleFPS();

					return IEventInterface::EVENT_PROCESSED; // filtered
				}
			break;
		}

		return IEventInterface::EVENT_IGNORED;
	}

	LRESULT GraphicsCore::OnActivate(bool bActive_in, bool bMinimized_in)
	{
#ifdef _M_IX86
		if (g_pDeviceWrapperImpl != nullptr && (g_pDeviceWrapperImpl->IsFullscreen() || bMinimized_in))
		{
			g_pDeviceWrapperImpl->SetRendering(bActive_in);

			return IEventInterface::EVENT_PROCESSED; // filtered
		}
#endif // _M_IX86

		return IEventInterface::EVENT_IGNORED;
	}

	void GraphicsCore::InitializeLabel(UiTextLabel *pLabel)
	{
		if (pLabel != nullptr)
		{
			unsigned long TextColor = 0xFFFF0000, ID = pLabel->GetID();
			bool Bold = true, Italic = false, Collapsed = false;
			const string_t &name = pLabel->GetName();
			string_t FontName = _T("Arial");
			unsigned short FontSize = 12U;
			long X = -16L, Y = 0L;

			bool Deserialized = m_pEngine->DeserializeLabel(name, X, Y, TextColor, FontName, FontSize, Bold, Italic, Collapsed);

			if (Deserialized == false)
				m_pEngine->SerializeLabel(name, X, Y, TextColor, FontName, FontSize, Bold, Italic, Collapsed);

			pLabel->SetVisibile(false);
			// add it to the list of renderables
			if (g_pDeviceWrapperImpl != nullptr)
				g_pDeviceWrapperImpl->AddRenderable(ID, pLabel);

			m_UiElements[ID] = pLabel;
		}
	}
}