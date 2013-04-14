/**************************************************************************
	created		:	2013-04-12
	filename	: 	UiTextLabel.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __UITEXTLABEL_H__
#define __UITEXTLABEL_H__

struct IDirect3DDevice9;
class Font;

namespace Windower
{
	class TextLabelRenderer;

	class UiTextLabel : public UIAL::CUiWindow<>, public IRenderable
	{
	public:
		/*! \brief UiTextLabel constructor
			\param[in] ID_in : ID of the label
		*/
		explicit UiTextLabel(unsigned long ID_in, IDirect3DDevice9 *pDevice_in, const string_t& Name_in,
							 long X_in, long Y_in, long W_in, long H_in, const string_t &FontName_in,
							 unsigned short FontSize_in, bool bBold_in, bool bItalic_in, unsigned long ARGB_in,
							 TextLabelRenderer *pRenderer_in, bool Visibile_in = true);

		virtual void OnDeviceReset();
		virtual void OnDeviceLost();
		virtual void Release();
		virtual void Update();
		virtual bool Draw();

		void GetScreenPos(long &X_out, long &Y_out) const;
		void Move(long XOffset_in, long YOffset_in);
		bool HitTest(long X_in, long Y_in) const;
		Font* GetFont();

	protected:
		//! the font object of the label
		Font m_Font;
	};
}

#endif//__UITEXTLABEL_H__
