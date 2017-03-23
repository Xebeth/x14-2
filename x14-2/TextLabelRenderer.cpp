/**************************************************************************
	created		:	2013-04-12
	filename	: 	TextLabelRenderer.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>

#include "Font.h"
#include "IRenderable.h"
#include "TextLabelRenderer.h"

#include "UiTextLabel.h"

namespace Windower
{
	TextLabelRenderer::TextLabelRenderer(WORD Width_in, WORD Height_in)
		: m_pFont(nullptr), m_Width(Width_in), m_Height(Height_in) {}

	void TextLabelRenderer::BeforeRender(UIAL::CUiWindow<> &Window_in_out)
	{
		m_pFont = static_cast<UiTextLabel&>(Window_in_out).GetFont();
	}

	bool TextLabelRenderer::Render(UIAL::CUiWindow<> &Window_in_out)
	{
		if (m_pFont != nullptr)
		{
			unsigned long W, H;
			long X, Y;

			// retrieve the size and position of the window
			Window_in_out.GetSize(W, H);
			Window_in_out.GetPos(X, Y);
			// move the negative coordinates to the right of the screen
			NormalizePosition(X, Y, W, H);

			// print the text
			m_pFont->Print(Window_in_out.GetTitleText().c_str(), X, Y,
						   Window_in_out.GetTitleColor().GetARGB(), W, H);

			return true;
		}

		return false;
	}

	void TextLabelRenderer::AfterRender(UIAL::CUiWindow<> &Window_in_out)
	{
		m_pFont = nullptr;
	}

	void TextLabelRenderer::NormalizePosition(long &X_in_out, long &Y_in_out,
											  long Width_in, long Height_in)
	{
		if (X_in_out < 0L || Y_in_out < 0L)
		{
			if (X_in_out < 0L)
				X_in_out = m_Width + X_in_out - Width_in;

			if (Y_in_out < 0L)
				Y_in_out = m_Height + Y_in_out - Height_in;
		}
	}

	void TextLabelRenderer::SetWindowSize(WORD Width_in, WORD Height_in)
	{
		m_Height = Height_in;
		m_Width = Width_in;
	}
}