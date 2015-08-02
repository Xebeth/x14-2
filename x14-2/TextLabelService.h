/**************************************************************************
	created		:	2013-04-11
	filename	: 	TextLabelService.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Text label creation service
**************************************************************************/
#ifndef __TEXT_LABEL_SERVICE_H__
#define __TEXT_LABEL_SERVICE_H__

typedef struct IDirect3DDevice9 IDirect3DDevice9;

namespace Windower
{
	typedef std::map<unsigned long, UiTextLabel*> UiLabelMap;
	typedef hash_map<TCHAR, unsigned long> UiIdMap;

	class TextLabelRenderer;
	class WindowerEngine;
	class UiTextLabel;

	class TextLabelService : public ModuleService
	{
	public:
		TextLabelService(WindowerEngine &Engine_in, const string_t& Name_in,
						 RenderableMap &UiElements_in, unsigned long BaseID_in,
						 bool InvokePermission_in = false);

		void SetRenderer(TextLabelRenderer *pRenderer_in, IDirect3DDevice9 *pDevice_in,
						 Direct3DDevice9WrapperImpl *pWrapperImpl_in);
		bool Invoke(PluginFramework::ServiceParam &Params_in);

	private:
		UiTextLabel* CreateLabel(IDirect3DDevice9 *pDevice_in, Direct3DDevice9WrapperImpl *pWrapperImpl_in,
								 const string_t& Name_in, long X_in, long Y_in, long W_in, long H_in,
								 const string_t &FontName_in, unsigned short FontSize_in,
								 bool bBold_in, bool bItalic_in, unsigned long ARGB_in, 
								 TextLabelRenderer *pRenderer_in, bool Visibile_in = true);
		bool DestroyLabel(unsigned long ID_in, Direct3DDevice9WrapperImpl *pWrapperImpl_in);

		Direct3DDevice9WrapperImpl *m_pWrapperImpl;
		TextLabelRenderer *m_pRenderer;
		IDirect3DDevice9 *m_pDevice;
		RenderableMap &m_UiElements;
		UiLabelMap m_RemovedLabels;
		WindowerEngine &m_Engine;
		unsigned long m_NextID;
		UiIdMap m_LabelIDs;
	};
}

#endif//__TEXT_LABEL_SERVICE_H__
