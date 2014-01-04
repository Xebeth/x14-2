/**************************************************************************
	created		:	2013-04-06
	filename	: 	PlayerDataService.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Player data service
**************************************************************************/
#include "stdafx.h"

#include "ModuleService.h"
#include "PlayerDataService.h"

namespace Windower
{
	/*! \brief PlayerDataService constructor
		\param[in] Name_in : the name of the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
	*/
	PlayerDataService::PlayerDataService(const string_t& Name_in, bool InvokePermission_in)
		: ModuleService(Name_in, InvokePermission_in), m_pPlayerData(NULL), m_pPlayerTarget(NULL)
	{
		// add compatible plugins
		StringUtils::UUID PluginUUID;

		m_CompatiblePlugins.insert(PluginUUID.FromString(_T("F4F02060-9ED0-11E2-9E96-0800200C9A66")));	// Distance
	}

	//! Creates the calling context for the service
	void PlayerDataService::CreateContext()
	{
		OnPlayerPtrChange(m_pPlayerData);
	}

	void PlayerDataService::OnPlayerPtrChange(const TargetData **pPlayerData_in)
	{
		m_pPlayerData = pPlayerData_in;
		NotifyPlugins();
	}

	void PlayerDataService::OnTargetPtrChange(const TargetData **pTargetData_in)
	{
		m_pPlayerTarget = pTargetData_in;
		NotifyPlugins();
	}

	void PlayerDataService::NotifyPlugins()
	{
		PluginFramework::PluginSet::const_iterator PluginIt, EndIt = m_Subscribers.cend();
		IPlayerDataPlugin *pPlugin = NULL;

		for (PluginIt = m_Subscribers.cbegin(); PluginIt != EndIt; ++PluginIt)
		{
			pPlugin = static_cast<IPlayerDataPlugin*>(*PluginIt);

			if (pPlugin != NULL)
				pPlugin->OnTargetPtrChange(m_pPlayerData, m_pPlayerTarget);
		}
	}

}