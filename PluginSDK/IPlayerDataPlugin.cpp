/**************************************************************************
	created		:	2013-04-06
	filename	: 	IPlayerDataPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "ConfigurablePlugin.h"
#include "WindowerPlugin.h"
#include "IPlayerDataPlugin.h"

#define PLAYER_DATA_MODULE			_T("PlayerData")
// available services for the module
#define GET_SELECTED_TARGET_HOOK	_T("GetSelectedTarget")
#define INIT_CHARACTER_MGR_HOOK		_T("CharacterMgrInit")

namespace Windower
{
	/*! \brief Adds the plugin as a subscriber to the game chat service
		\return true if the subscription succeeded; false otherwise
	*/
	bool IPlayerDataPlugin::Subscribe()
	{
		return SubscribeService(PLAYER_DATA_MODULE, INIT_CHARACTER_MGR_HOOK);
	}

	/*! \brief Removes the plugin as a subscriber to the game chat service
		\return true if the subscription was revoked successfully; false otherwise
	*/
	bool IPlayerDataPlugin::Unsubscribe()
	{
		return UnsubscribeService(PLAYER_DATA_MODULE, INIT_CHARACTER_MGR_HOOK);
	}
}
