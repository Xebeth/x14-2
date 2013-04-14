/**************************************************************************
	created		:	2013-04-12
	filename	: 	UiFPSCounter.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <d3dx9.h>
#include <d3d9.h>

#include "Font.h"
#include "Timer.h"
#include "IRenderable.h"
#include "TextLabelRenderer.h"
#include "UiTextLabel.h"

#include "UiFPSCounter.h"

namespace Windower
{
	UiFPSCounter::UiFPSCounter(unsigned long ID_in, IDirect3DDevice9 *pDevice_in,
								 const string_t& Name_in, long X_in, long Y_in, long W_in,
								 long H_in, const string_t &FontName_in, unsigned short FontSize_in,
								 bool bBold_in, bool bItalic_in, unsigned long ARGB_in,
								 TextLabelRenderer *pRenderer_in, bool Visibile_in)
		: UiTextLabel(ID_in, pDevice_in, Name_in, X_in, Y_in, W_in, H_in, FontName_in, FontSize_in,
					  bBold_in, bItalic_in, ARGB_in, pRenderer_in, Visibile_in), m_pRenderTimer(NULL)
	{
		m_pRenderTimer = new Timer;
		m_pRenderTimer->Start();
	}

	UiFPSCounter::~UiFPSCounter()
	{
		if (m_pRenderTimer != NULL)
		{
			delete m_pRenderTimer;
			m_pRenderTimer = NULL;
		}
	}

	void UiFPSCounter::Update()
	{
		string_t FPS;

		// update the timer
		m_pRenderTimer->Update();

		// retrieve the FPS counter and update the label
		format(FPS, _T("%.2f fps"), m_pRenderTimer->GetFPS() * 0.33333333f);
		SetTitleText(FPS);
	}
}