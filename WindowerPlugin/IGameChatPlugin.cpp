/**************************************************************************
	created		:	2011-11-27
	filename	: 	IGameChatPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>

#include "IGameChatPlugin.h"

namespace Windower
{
	/*! \brief Adds the plugin as a subscriber to the game chat service
		\return true if the subscription succeeded; false otherwise
	*/
	bool IGameChatPlugin::Subscribe()
	{
		return IPlugin::Services()->SubscribeService(_T("GameChat"), _T("OnChatMessage"), this);
	}

	/*! \brief Removes the plugin as a subscriber to the game chat service
		\return true if the subscription was revoked successfully; false otherwise
	*/
	bool IGameChatPlugin::Unsubscribe()
	{
		return IPlugin::Services()->UnsubscribeService(_T("GameChat"), _T("OnChatMessage"), this);
	}
}
