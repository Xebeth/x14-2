/**************************************************************************
	created		:	2013-04-06
	filename	: 	IPlayerDataPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "PlayerDataTypes.h"
#include "WindowerPlugin.h"
#include "IPlayerDataPlugin.h"

namespace Windower
{
	/*! \brief Adds the plugin as a subscriber to the game chat service
		\return true if the subscription succeeded; false otherwise
	*/
	bool IPlayerDataPlugin::Subscribe()
	{
		return SubscribeService(_T(PLAYER_DATA_SERVICE), _T(INIT_CHARACTER_MGR_HOOK));
	}

	/*! \brief Removes the plugin as a subscriber to the game chat service
		\return true if the subscription was revoked successfully; false otherwise
	*/
	bool IPlayerDataPlugin::Unsubscribe()
	{
		return UnsubscribeService(_T(PLAYER_DATA_SERVICE), _T(INIT_CHARACTER_MGR_HOOK));
	}
}
