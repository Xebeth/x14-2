/**************************************************************************
	created		:	2011-10-18
	filename	: 	InjectVersion.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Engine internal plugin used to inject the windower version in the main menu screen
**************************************************************************/
#include "stdafx.h"
#include "version.h"
#include <PluginFramework.h>

#include "ICreateTextNodePlugin.h"
#include "InjectVersion.h"

const PluginFramework::IPluginServices* PluginFramework::IPlugin::m_pPluginServices = NULL;

namespace Windower
{
	/*! \brief InjectVersion constructor
		\param[in] pPluginServices_in : the plugin services
	*/
	InjectVersion::InjectVersion(PluginFramework::IPluginServices *pPluginServices_in)
		: ICreateTextNodePlugin("Game Version:")
	{
		m_pPluginServices = pPluginServices_in;
		m_PluginInfo.Descritpion = _T("");
		m_PluginInfo.Author = _T("Xebeth`");
		m_PluginInfo.Name = _T("InjectVersion");
		m_PluginInfo.PluginVersion.FromString(_T("1.0.0"));
		m_PluginInfo.FrameworkVersion.FromString(_T("1.0.0"));
		m_PluginInfo.PluginIdentifier.FromString(_T("932E0F5D-1D24-40B1-BA63-D729A6E42C90"));
		m_pPluginServices->SubscribeService(_T("GameChat"), _T("CreateTextNode"), this);
	}

	/*! \brief Callback function invoked when the game creates a string object
		\param[in] pText_in : the text used to create the string node
		\param[in] Unsubscribe_out : flag specifying if the plugin wants to revoke its subscription to the hook
		\return the modified version of the text
	*/
	const char* InjectVersion::OnCreateTextNode(const char *pText_in, bool &Unsubscribe_out)
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
}
