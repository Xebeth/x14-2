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

namespace Windower
{
	/*! \brief InjectVersion constructor
		\param[in] pServices_in : a pointer to the plugin services
	*/
	InjectVersion::InjectVersion(PluginFramework::IPluginServices *pServices_in)
		: ICreateTextNodePlugin(pServices_in, "Game Version:")
	{
		m_PluginInfo.SetIdentifier(_T("932E0F5D-1D24-40B1-BA63-D729A6E42C90"));
		m_PluginInfo.SetName(_T("InjectVersion"));
		m_PluginInfo.SetAuthor(_T("Xebeth`"));
		m_PluginInfo.SetVersion(_T("1.0.0"));

		Subscribe();
	}

	//! \brief InjectVersion destructor
	InjectVersion::~InjectVersion()
	{
		Unsubscribe();
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
#ifdef _DEBUG
			format(m_InjectedText, "%s\nWindower x14 Version: %i.%i.%i.%i debug build",
#else
			format(m_InjectedText, "%s\nWindower x14 Version: %i.%i.%i.%i",
#endif // _DEBUG
				   pText_in, MODULE_MAJOR_VERSION, MODULE_MINOR_VERSION,
				   MODULE_RELEASE_VERSION, MODULE_TEST_VERSION);
			// only need to inject once
			Unsubscribe_out = true;

			return m_InjectedText.c_str();
		}

		return pText_in;
	}

	/*! \brief Adds the plugin as a subscriber to the game chat service
		\return true if the subscription succeeded; false otherwise
	*/
	bool InjectVersion::Subscribe()
	{
		return SubscribeService(_T("GameChat"), _T("CreateTextNode"));
	}

	/*! \brief Removes the plugin as a subscriber to the game chat service
		\return true if the subscription was revoked successfully; false otherwise
	*/
	bool InjectVersion::Unsubscribe()
	{
		return UnsubscribeService(_T("GameChat"), _T("CreateTextNode"));
	}
}
