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
	class UiTextLabel;

	class UiTextLabel : public IRenderable, public UIAL::CUiWindow<>
	{
	public:
		/*! \brief UiTextLabel constructor
			\param[in] ID_in : ID of the label
		*/
		explicit UiTextLabel(unsigned long ID_in, IDirect3DDevice9 *pDevice_in, const string_t& Name_in,
							 long X_in, long Y_in, long W_in, long H_in, const string_t &FontName_in,
							 unsigned short FontSize_in, bool bBold_in, bool bItalic_in, unsigned long ARGB_in,
							 TextLabelRenderer *pRenderer_in, bool Visibile_in = true);

		virtual void OnMouseClick(WORD X_in, WORD Y_in, DWORD MouseFlags_in, UINT KeyFlags_in);
		virtual void SetVisible(bool Visible_in);
		virtual bool IsVisible() const;
		virtual void OnDeviceReset();
		virtual void OnDeviceLost();
		virtual void Release();
		virtual void Update();
		virtual bool Draw();

		void SetMouseClickEvent(fnOnMouseClick pEvent) { m_pfnMouseClickEvent = pEvent; }
		void GetScreenPos(long &X_out, long &Y_out) const;
		void Move(long XOffset_in, long YOffset_in);
		bool HitTest(long X_in, long Y_in) const;
		Font* GetFont();

	protected:
		fnOnMouseClick m_pfnMouseClickEvent;
		//! the font object of the label
		Font m_Font;
	};
}

#endif//__UITEXTLABEL_H__
