/**************************************************************************
	created		:	2011-11-27
	filename	: 	ICreateTextNodePlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Base class for plugins subscribing to the CreateTextNode service
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>

#include "ICreateTextNodePlugin.h"

namespace Windower
{
	/*! \brief Adds the plugin as a subscriber to the game chat service
		\return true if the subscription succeeded; false otherwise
	*/
	bool ICreateTextNodePlugin::Subscribe()
	{
		return (SubscribeService(_T("GameChat"), _T("CreateTextNode"))
			 && SubscribeService(_T("GameChat"), _T("OnChatMessage")));
	}

	/*! \brief Removes the plugin as a subscriber to the game chat service
		\return true if the subscription was revoked successfully; false otherwise
	*/
	bool ICreateTextNodePlugin::Unsubscribe()
	{
		return (UnsubscribeService(_T("GameChat"), _T("CreateTextNode"))
			 && UnsubscribeService(_T("GameChat"), _T("OnChatMessage")));
	}
}
