/**************************************************************************
	created		:	2010-10-10
	filename	: 	GraphicsCore.h
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interface with Direct3D 9
**************************************************************************/
#ifndef __GRAPHICS_CORE_H__
#define __GRAPHICS_CORE_H__

class Direct3DDevice9WrapperImpl;
class Direct3D9WrapperImpl;
class IDirect3D9Wrapper;

#define GRAPHICS_MODULE			"Graphics"
#define TEXT_LABEL_SERVICE		"TextLabelService"

typedef struct IDirect3D9 Direct3D9;
typedef struct IDirect3DDevice9 IDirect3DDevice9;

typedef IDirect3D9*	(WINAPI *fnDirect3DCreate9)(UINT SDKVersion_in);

namespace Windower
{
	class TextLabelRenderer;
	class WindowerEngine;
	class UiTextLabel;
	
	class GraphicsCore : public WindowerCore, public IEventInterface, public IDeviceCreateSubscriber
	{
		enum eStaticLabels
		{
			GFX_TEXT_FPS = 0,	// text label for the FPS counter
			GFX_TEXT_COUNT		// number of static text labels
		};

	public:
		explicit GraphicsCore(bool VSync_in);
		~GraphicsCore();

		void Detach();

		BaseModuleService* CreateService(const string_t& ServiceName_in, bool InvokePermission_in = false);
		bool Invoke(const string_t& ServiceName_in, PluginFramework::ServiceParam &Params_in);

		void OnDeviceCreate(IDirect3DDevice9 *pDevice_in, const D3DPRESENT_PARAMETERS *pPresentParams_in);

		LRESULT OnLButtonDown(WORD X_in, WORD Y_in, DWORD MouseFlags_in, UINT KeyFlags_in);
		LRESULT OnMouseMove(WORD X_in, WORD Y_in, DWORD MouseFlags_in, UINT KeyFlags_in);
		LRESULT OnLButtonUp(WORD X_in, WORD Y_in, DWORD MouseFlags_in, UINT KeyFlags_in);
		LRESULT OnKeyUp(UINT PressedChar_in, UINT RepetitionCount_in, UINT KeyFlags_in);
		LRESULT OnSize(int NewWidth_in, int NewHeight_in, UINT nFlags_in);
		UiTextLabel* HitTest(WORD X_in, WORD Y_in);

		void SetRendering(bool bEnable_in);
		bool RegisterServices();
		void ToggleRendering();
		void ToggleFPS();

		// ICoreModule interface implementation
		void RegisterHooks(HookEngineLib::IHookManager &HookManager_in);
		void OnHookInstall(HookEngineLib::IHookManager &HookManager_in);

		// Direct3D
		static IDirect3D9* WINAPI Direct3DCreate9Hook(UINT SDKVersion_in);

	protected:
		TextLabelRenderer* GetLabelRenderer();

		//! function pointer to the original Direct3DCreate9 function
		fnDirect3DCreate9 m_pDirect3DCreate9Trampoline;
		//! Direct3D device
		IDirect3DDevice9 *m_pDirect3DDevice;
		//! flag specifying if vertical synchronization is in use
		bool m_VSync;
		//! graphical elements
		RenderableMap m_UiElements;
		//! text label renderer
		TextLabelRenderer *m_pLabelRenderer;
		//! game window size
		WORD m_Width, m_Height;
		//! mouse position
		long m_MouseOffsetX, m_MouseOffsetY;
		DWORD m_LabelWidth, m_LabelHeight;
		//! pointer to the label being moved
		UiTextLabel *m_pMovingLabel;
		//! calling context for the service hooks
		static CallingContext<GraphicsCore> m_Context;
	};
}

#endif//__GRAPHICS_CORE_H__