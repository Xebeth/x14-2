/**************************************************************************
	created		:	2013-04-12
	filename	: 	UiTextLabel.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <d3dx9.h>
#include <d3d9.h>

#include "Font.h"
#include "IRenderable.h"
#include "IDirect3DDevice9Wrapper.h"

#include "UiTextLabel.h"

namespace Windower
{
	UiTextLabel::UiTextLabel(unsigned long ID_in, IDirect3DDevice9 *pDevice_in, const string_t& Name_in,
							 long X_in, long Y_in, long W_in, long H_in, const string_t &FontName_in,
							 unsigned short FontSize_in, bool bBold_in, bool bItalic_in, unsigned long ARGB_in,
							 UIAL::IWindowRenderer<> *pRenderer_in, bool Visibile_in)
		: UIAL::CUiWindow<>(ID_in, Name_in, X_in, Y_in, W_in, H_in, Visibile_in, _T(""), UIAL::CUiFont(FontName_in, FontSize_in),
							UIAL::CUiColor(ARGB_in), UIAL::CUiColor(0xFFFFFFFF), pRenderer_in), m_pDevice(pDevice_in)
	{
		m_Font.Initialize(pDevice_in, FontName_in.c_str(), FontSize_in, true, bBold_in, bItalic_in);
		NormalizePosition();
	}

	void UiTextLabel::OnDeviceReset()
	{
		m_Font.OnDeviceReset();
	}

	void UiTextLabel::OnDeviceLost()
	{
		m_Font.OnLostDevice();
	}

	void UiTextLabel::Release()
	{
		m_Font.Release();
	}

	/*! \brief Retrieves the font object of the label
		\return the font object of the label
	*/
	Font* UiTextLabel::GetFont()
	{ return &m_Font; }

	void UiTextLabel::Update() {}

	bool UiTextLabel::Draw()
	{ return CUiWindow::Draw(); }

	void UiTextLabel::NormalizePosition()
	{
		if (m_pDevice != NULL)
		{
			long X = m_Rect.GetX();
			long Y = m_Rect.GetY();

			if (X < 0L || Y < 0L)
			{
				D3DVIEWPORT9 Viewport;

				m_pDevice->GetViewport(&Viewport);

				if (X < 0L)
					m_Rect.SetX(Viewport.Width + X - m_Rect.GetWidth());

				if (Y < 0L)
					m_Rect.SetY(Viewport.Height + Y - m_Rect.GetHeight());
			}
		}
	}
}