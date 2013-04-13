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
	class TextLabelRenderer;

	class TextLabelService : public ModuleService
	{
	public:
		TextLabelService(const string_t& Name_in, RenderableMap &UiElements_in,
						 unsigned long BaseID_in, bool InvokePermission_in = false);

		void SetRenderer(IDirect3DDevice9Wrapper *pDevice_in, TextLabelRenderer *pRenderer_in);
		bool Invoke(const PluginFramework::ServiceParam &Params_in);

	private:
		IDirect3DDevice9Wrapper *m_pDevice;
		TextLabelRenderer *m_pRenderer;
		RenderableMap &m_UiElements;
		unsigned long m_NextID;
	};
}

#endif//__TEXT_LABEL_SERVICE_H__
