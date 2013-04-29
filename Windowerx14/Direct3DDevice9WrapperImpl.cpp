/**************************************************************************
	created		:	2013-04-29
	filename	: 	Direct3DDevice9WrapperImpl.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <d3d9.h>

#include "IRenderable.h"
#include "Direct3DDevice9WrapperImpl.h"

Direct3DDevice9WrapperImpl::Direct3DDevice9WrapperImpl(const D3DPRESENT_PARAMETERS &PresentParams_in)
	: m_bSceneStarted(false), m_bDrawUi(false), m_bFullscreen(false),
	  m_bRender(true), m_PresentParams(PresentParams_in) {}

bool Direct3DDevice9WrapperImpl::AddRenderable(unsigned long ID_in, IRenderable *pRenderable_in)
{
	if (pRenderable_in != NULL)
	{
		m_UiElements[ID_in] = pRenderable_in;

		return true;
	}

	return false;
}

bool Direct3DDevice9WrapperImpl::RemoveRenderable(unsigned long ID_in)
{
	RenderableMap::const_iterator RenderableIt = m_UiElements.find(ID_in);
	bool Result = false, DrawUi = m_bDrawUi;

	while (m_bDrawUi && m_bSceneStarted)
		Sleep(0);

	m_bDrawUi = false;

	if (RenderableIt != m_UiElements.end())
	{
		m_UiElements.erase(RenderableIt);

		Result = true;
	}

	m_bDrawUi = DrawUi;

	return Result;
}

void Direct3DDevice9WrapperImpl::Draw()
{
	m_bSceneStarted = true;
	m_bDrawUi = !m_bDrawUi;

	if (m_bDrawUi)
	{
		RenderableMap::const_iterator RenderableIt, EndIt = m_UiElements.cend();
		IRenderable *pRenderable = NULL;

		for(RenderableIt = m_UiElements.cbegin(); RenderableIt != EndIt; ++RenderableIt)
		{
			pRenderable = RenderableIt->second;

			if (pRenderable != NULL && pRenderable->IsVisible())
			{
				pRenderable->Update();
				pRenderable->Draw();
			}
		}
	}
}

void Direct3DDevice9WrapperImpl::OnDeviceLost(D3DPRESENT_PARAMETERS &PresentationParameters_in_out)
{
	RenderableMap::const_iterator RenderableIt, EndIt = m_UiElements.cend();
	IRenderable *pRenderable = NULL;

	// force vertical sync
	PresentationParameters_in_out.PresentationInterval = m_PresentParams.PresentationInterval;
	PresentationParameters_in_out.BackBufferCount = m_PresentParams.BackBufferCount;
	PresentationParameters_in_out.SwapEffect = m_PresentParams.SwapEffect;
	m_bFullscreen = (PresentationParameters_in_out.Windowed == FALSE);

	for(RenderableIt = m_UiElements.cbegin(); RenderableIt != EndIt; ++RenderableIt)
	{
		pRenderable = RenderableIt->second;

		if (pRenderable != NULL)
			pRenderable->OnDeviceLost();
	}
}

void Direct3DDevice9WrapperImpl::OnDeviceReset()
{
	RenderableMap::const_iterator RenderableIt, EndIt = m_UiElements.cend();
	IRenderable *pRenderable = NULL;

	for(RenderableIt = m_UiElements.cbegin(); RenderableIt != EndIt; ++RenderableIt)
	{
		pRenderable = RenderableIt->second;

		if (pRenderable != NULL)
			pRenderable->OnDeviceReset();
	}
}