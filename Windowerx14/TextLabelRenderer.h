/**************************************************************************
	created		:	2013-04-12
	filename	: 	TextLabelRenderer.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __TEXT_LABEL_RENDERER_H__
#define __TEXT_LABEL_RENDERER_H__

class Font;

namespace Windower
{
	class TextLabelRenderer : public UIAL::IWindowRenderer<>
	{
	public:
		TextLabelRenderer() : m_pFont(NULL) {}

		void BeforeRender(UIAL::CUiWindow<> &Window_in_out);
		void AfterRender(UIAL::CUiWindow<> &Window_in_out);
		bool Render(UIAL::CUiWindow<> &Window_in_out);

	protected:
		Font *m_pFont;
	};
}

#endif//__TEXTLABELRENDERER_H__
