/**************************************************************************
	created		:	2013-04-11
	filename	: 	TextLabelService.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Text label creation service
**************************************************************************/
#ifndef __TEXT_LABEL_SERVICE_H__
#define __TEXT_LABEL_SERVICE_H__

class IDirect3DDevice9Wrapper;

namespace Windower
{
	typedef std::map<unsigned long, UiTextLabel*> UiLabelMap;
	typedef std::hash_map<string_t, unsigned long> UiIdMap;

	class TextLabelRenderer;
	class UiTextLabel;

	class TextLabelService : public ModuleService
	{
	public:
		TextLabelService(const string_t& Name_in, RenderableMap &UiElements_in,
						 unsigned long BaseID_in, bool InvokePermission_in = false);

		void SetRenderer(IDirect3DDevice9Wrapper *pDevice_in, TextLabelRenderer *pRenderer_in);
		UiTextLabel* CreateLabel(IDirect3DDevice9Wrapper *pDevice_in, const string_t& Name_in, long X_in, long Y_in,
								 long W_in, long H_in, const string_t &FontName_in, unsigned short FontSize_in,
								 bool bBold_in, bool bItalic_in, unsigned long ARGB_in, 
								 TextLabelRenderer *pRenderer_in, bool Visibile_in = true);
		bool DestroyLabel(unsigned long ID_in, IDirect3DDevice9Wrapper *pDevice_in);
		bool Invoke(const PluginFramework::ServiceParam &Params_in);

	private:
		IDirect3DDevice9Wrapper *m_pDevice;
		TextLabelRenderer *m_pRenderer;
		RenderableMap &m_UiElements;
		UiLabelMap m_RemovedLabels;
		unsigned long m_NextID;
		UiIdMap m_LabelIDs;
	};
}

#endif//__TEXT_LABEL_SERVICE_H__
