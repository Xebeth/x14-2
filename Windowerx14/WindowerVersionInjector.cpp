/**************************************************************************
	created		:	2011-10-18
	filename	: 	WindowerVersionInjector.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include "version.h"
#include <PluginFramework.h>

#include "IAllocStringPlugin.h"
#include "WindowerVersionInjector.h"

using namespace Windower;

const PluginFramework::IPluginServices* PluginFramework::IPlugin::m_pPluginServices = NULL;

Windower::WindowerVersionInjector::WindowerVersionInjector(PluginFramework::IPluginServices *pPluginServices_in)
	: IAllocStringPlugin("Game Version:")
{
	m_pPluginServices = pPluginServices_in;
	m_BasePluginInfo.Descritpion = _T("");
	m_BasePluginInfo.Author = _T("Xebeth`");
	m_BasePluginInfo.Name = _T("InjectVersion");
	m_BasePluginInfo.PluginVersion.FromString(_T("1.0.0"));
	m_BasePluginInfo.FrameworkVersion.FromString(_T("1.0.0"));
	m_BasePluginInfo.PluginIdentifier.FromString(_T("932E0F5D-1D24-40B1-BA63-D729A6E42C90"));
	m_pPluginServices->SubscribeService(_T("GameChat"), _T("AllocString"), this);
}

const char* WindowerVersionInjector::OnAllocString(const char *pText_in, bool &Unsubscribe_out)
{
	Unsubscribe_out = false;

	if (m_pTargetPtr == NULL && strstr(pText_in, m_TargetText.c_str()) != NULL)
		m_pTargetPtr = pText_in;

	if (pText_in == m_pTargetPtr)
	{
		format(m_InjectedText, "%s\nWindower x14 Version: %i.%i.%i.%i",
			   pText_in, MODULE_MAJOR_VERSION, MODULE_MINOR_VERSION,
			   MODULE_RELEASE_VERSION, MODULE_TEST_VERSION);

		// only need to inject once
		Unsubscribe_out = true;

		return m_InjectedText.c_str();
	}

	return pText_in;
}
