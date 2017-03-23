/**************************************************************************
	created		:	2013-04-12
	filename	: 	UiFPSCounter.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>

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
					  bBold_in, bItalic_in, ARGB_in, pRenderer_in, Visibile_in), m_pRenderTimer(nullptr)

	{
		m_pRenderTimer = new Timer;
		m_pRenderTimer->Start();
	}

	UiFPSCounter::~UiFPSCounter()
	{
		if (m_pRenderTimer != nullptr)
		{
			delete m_pRenderTimer;
			m_pRenderTimer = nullptr;
		}
	}
	
	void UiFPSCounter::Update()
	{
		string_t FPS;

		// update the timer
		m_pRenderTimer->Update();
		// retrieve the FPS counter and update the label
		format(FPS, _T("%.2f fps"), m_pRenderTimer->GetFPS());
		SetTitleText(FPS);
	}

	UiTimerCounter::UiTimerCounter(unsigned long ID_in, IDirect3DDevice9 *pDevice_in,
								   const string_t& Name_in, long X_in, long Y_in, long W_in,
								   long H_in, const string_t &FontName_in, unsigned short FontSize_in,
								   bool bBold_in, bool bItalic_in, unsigned long ARGB_in,
								   TextLabelRenderer *pRenderer_in, bool Visibile_in)
		: UiFPSCounter(ID_in, pDevice_in, Name_in, X_in, Y_in, W_in, H_in, FontName_in, FontSize_in,
					   bBold_in, bItalic_in, ARGB_in, pRenderer_in, Visibile_in), m_Step(0UL), m_Total(0UL) {}

	void UiTimerCounter::SetProgress(unsigned long step, unsigned long total, bool stop)
	{
		m_Total = total;
		m_Step = step;

		if (m_pRenderTimer != nullptr)
		{
			if (m_Step == 0UL)
			{
				m_pRenderTimer->Reset();
				m_pRenderTimer->Start();
			}
			else if (m_Step > m_Total)
			{
				m_pRenderTimer->Stop();
				m_Step = m_Total;
			}
			else if (m_Step == 2UL)
			{
				unsigned long time = (unsigned long)m_pRenderTimer->GetRunningTime() * m_Total;
				unsigned long seconds = time % 60;
				unsigned long minutes = (time / 60) % 60;
				unsigned long hours = (time / 3600);

				format(m_EstimatedTime, _T("%02d:%02d:%02d"), hours, minutes, seconds);
			}
		}
	}

	void UiTimerCounter::Update()
	{
		if (m_Total != 0UL)
		{
			string_t text;

			// update the timer
			m_pRenderTimer->Update();

			format(text, _T("Macro progress: %d / %d (%0.2f%%) - %s"),
							m_Step, m_Total, 100.f * m_Step / m_Total,
							m_pRenderTimer->RunningTime(string_t()).c_str());

			if (m_EstimatedTime.empty() == false)
				append_format(text, _T(" (%s)"), m_EstimatedTime.c_str());

			SetTitleText(text);
		}
	}
}