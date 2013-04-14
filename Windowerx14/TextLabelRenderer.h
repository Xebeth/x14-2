/**************************************************************************
	created		:	2013-04-12
	filename	: 	TextLabelRenderer.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __TEXT_LABEL_RENDERER_H__
#define __TEXT_LABEL_RENDERER_H__

struct IDirect3DDevice9;
class Font;

namespace Windower
{
	class TextLabelRenderer : public UIAL::IWindowRenderer<>
	{
	public:
		TextLabelRenderer(WORD Width_in, WORD Height_in);

		void BeforeRender(UIAL::CUiWindow<> &Window_in_out);
		void AfterRender(UIAL::CUiWindow<> &Window_in_out);
		bool Render(UIAL::CUiWindow<> &Window_in_out);

		void SetWindowSize(WORD Width_in, WORD Height_in);

		void NormalizePosition(long &X_in_out, long &Y_in_out,
							   long Width_in, long Height_in);

protected:
		WORD m_Width, m_Height;
		Font *m_pFont;
	};
}

#endif//__TEXTLABELRENDERER_H__
