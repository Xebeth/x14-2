/**************************************************************************
	created		:	2011-11-27
	filename	: 	IGameChatPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "WindowerPlugin.h"
#include "IGameChatPlugin.h"

#define GAME_CHAT_MODULE			"GameChat"
// available services for the module
#define FORMAT_CHAT_MESSAGE_HOOK	"FormatChatMessage"

namespace Windower
{
	/*! \brief Adds the plugin as a subscriber to the game chat service
		\return true if the subscription succeeded; false otherwise
	*/
	bool IGameChatPlugin::Subscribe()
	{
		return SubscribeService(_T(GAME_CHAT_MODULE), _T(FORMAT_CHAT_MESSAGE_HOOK));
	}

	/*! \brief Removes the plugin as a subscriber to the game chat service
		\return true if the subscription was revoked successfully; false otherwise
	*/
	bool IGameChatPlugin::Unsubscribe()
	{
		return UnsubscribeService(_T(GAME_CHAT_MODULE), _T(FORMAT_CHAT_MESSAGE_HOOK));
	}
}
