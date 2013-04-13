/**************************************************************************
	created		:	2013-04-12
	filename	: 	TextLabelRenderer.cpp
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
	void TextLabelRenderer::BeforeRender(UIAL::CUiWindow<> &Window_in_out)
	{
		m_pFont = static_cast<UiTextLabel&>(Window_in_out).GetFont();
	}

	bool TextLabelRenderer::Render(UIAL::CUiWindow<> &Window_in_out)
	{
		if (m_pFont != NULL)
		{
			unsigned long W, H;
			long X, Y;

			// retrieve the size and position of the window
			Window_in_out.GetSize(W, H);
			Window_in_out.GetPos(X, Y);
			// print the text
			m_pFont->Print(Window_in_out.GetTitleText().c_str(), X, Y,
						   Window_in_out.GetTitleColor().GetARGB(), W, H);

			return true;
		}

		return false;
	}

	void TextLabelRenderer::AfterRender(UIAL::CUiWindow<> &Window_in_out)
	{
		m_pFont = NULL;
	}
}