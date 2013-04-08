/**************************************************************************
	created		:	2013-04-06
	filename	: 	PlayerDataService.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Player data service
**************************************************************************/
#include "stdafx.h"

#include "PlayerDataService.h"

namespace Windower
{
	/*! \brief PlayerDataService constructor
		\param[in] Name_in : the name of the service
		\param[in] Hooks_in : the hooks associated with the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
	*/
	PlayerDataService::PlayerDataService(const string_t& Name_in, const HookPointers &Hooks_in, bool InvokePermission_in)
		: ModuleService(Name_in, Hooks_in, InvokePermission_in), m_pPlayerData(NULL), m_pPlayerTarget(NULL)
	{
		// add compatible plugins
		PluginFramework::PluginUUID UUID;

		m_CompatiblePlugins.insert(UUID.FromString(_T("F4F02060-9ED0-11E2-9E96-0800200C9A66")));	// Distance
	}

	//! Creates the calling context for the service
	void PlayerDataService::CreateContext()
	{
		OnPlayerPtrChange(m_pPlayerData);
	}

	void PlayerDataService::OnPlayerPtrChange(const TargetData *pPlayerData_in)
	{
		m_pPlayerData = pPlayerData_in;

		if (pPlayerData_in != NULL)
		{
			PluginFramework::PluginSet::const_iterator PluginIt;			
			IPlayerDataPlugin *pPlugin;

			for (PluginIt = m_Subscribers.begin(); PluginIt != m_Subscribers.end(); ++PluginIt)
			{
				pPlugin = static_cast<IPlayerDataPlugin*>(*PluginIt);

				if (pPlugin != NULL)
					pPlugin->OnPlayerPtrChange(m_pPlayerData);
			}
		}
	}

	void PlayerDataService::OnTargetPtrChange(const TargetData *pTargetData_in)
	{
		m_pPlayerTarget = pTargetData_in;

		if (pTargetData_in != NULL)
		{
			PluginFramework::PluginSet::const_iterator PluginIt;			
			IPlayerDataPlugin *pPlugin;

			for (PluginIt = m_Subscribers.begin(); PluginIt != m_Subscribers.end(); ++PluginIt)
			{
				pPlugin = static_cast<IPlayerDataPlugin*>(*PluginIt);

				if (pPlugin != NULL)
					pPlugin->OnTargetPtrChange(m_pPlayerTarget);
			}
		}
	}
}