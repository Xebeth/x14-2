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
#include "TextLabelRenderer.h"

#include "UiTextLabel.h"

namespace Windower
{
	UiTextLabel::UiTextLabel(unsigned long ID_in, IDirect3DDevice9 *pDevice_in, const string_t& Name_in,
							 long X_in, long Y_in, long W_in, long H_in, const string_t &FontName_in,
							 unsigned short FontSize_in, bool bBold_in, bool bItalic_in, unsigned long ARGB_in,
							 TextLabelRenderer *pRenderer_in, bool Visibile_in)
		: UIAL::CUiWindow<>(ID_in, Name_in, X_in, Y_in, W_in, H_in, Visibile_in, _T(""), UIAL::CUiFont(FontName_in, FontSize_in),
							UIAL::CUiColor(ARGB_in), UIAL::CUiColor(0xFFFFFFFF), pRenderer_in)
	{
		m_Font.Initialize(pDevice_in, FontName_in.c_str(), FontSize_in, true, bBold_in, bItalic_in);
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

	/*! \brief Checks if the coordinates are inside the element rectangle
		\param[in] X_in : the x-coordinate to check
		\param[in] Y_in : the y-coordinate to check
		\return true if the coordinates are inside the element rectangle; false otherwise
	*/
	bool UiTextLabel::HitTest(long X_in, long Y_in) const
	{
		UIAL::CUiWindow<>::CUiRect Rect = m_Rect;
		long X = 0L, Y = 0L;

		GetScreenPos(X, Y);
		Rect.SetX(X);
		Rect.SetY(Y);

		return IsInsideRect(Rect, X_in, Y_in);
	}

	void UiTextLabel::GetScreenPos(long &X_out, long &Y_out) const
	{
		TextLabelRenderer *pRenderer = static_cast<TextLabelRenderer*>(m_pRenderer);

		if (pRenderer != NULL)
		{
			X_out = m_Rect.GetX();
			Y_out = m_Rect.GetY();

			pRenderer->NormalizePosition(X_out, Y_out, m_Rect.GetWidth(), m_Rect.GetHeight());
		}
	}

	void UiTextLabel::Move(long XOffset_in, long YOffset_in)
	{
		long X = 0L, Y = 0L;

		GetScreenPos(X, Y);
		X += XOffset_in;
		Y += YOffset_in;

		if (X < 0L)
			XOffset_in -= X;
		if (Y < 0L)
			YOffset_in -= Y;

		__super::Move(XOffset_in, YOffset_in);
	}
}