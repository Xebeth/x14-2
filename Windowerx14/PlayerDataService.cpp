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

	void PlayerDataService::OnPlayerPtrChange(TargetData *pPlayerData_in)
	{
		PluginFramework::PluginSet::const_iterator PluginIt;			
		IPlayerDataPlugin *pPlugin;
		TargetPos PlayerData;

		m_pPlayerData = pPlayerData_in;

		if (m_pPlayerData != NULL)
		{
			PlayerData.pPosX = &pPlayerData_in->PosX;
			PlayerData.pPosY = &pPlayerData_in->PosY;
			PlayerData.pPosZ = &pPlayerData_in->PosZ;
			PlayerData.pTargetName = pPlayerData_in->Name;
		}

		for (PluginIt = m_Subscribers.begin(); PluginIt != m_Subscribers.end(); ++PluginIt)
		{
			pPlugin = static_cast<IPlayerDataPlugin*>(*PluginIt);

			if (pPlugin != NULL)
				pPlugin->OnPlayerPtrChange(PlayerData);
		}
	}

	void PlayerDataService::OnTargetPtrChange(TargetData *pTargetData_in)
	{
		if (m_pPlayerTarget != pTargetData_in)
		{
			PluginFramework::PluginSet::const_iterator PluginIt;
			IPlayerDataPlugin *pPlugin;
			TargetPos PlayerTarget;

			m_pPlayerTarget = pTargetData_in;

			if (pTargetData_in != NULL)
			{
				PlayerTarget.pPosX = &pTargetData_in->PosX;
				PlayerTarget.pPosY = &pTargetData_in->PosY;
				PlayerTarget.pPosZ = &pTargetData_in->PosZ;
				PlayerTarget.pTargetName = pTargetData_in->Name;
			}

			for (PluginIt = m_Subscribers.begin(); PluginIt != m_Subscribers.end(); ++PluginIt)
			{
				pPlugin = static_cast<IPlayerDataPlugin*>(*PluginIt);

				if (pPlugin != NULL)
					pPlugin->OnTargetPtrChange(PlayerTarget);
			}
		}
	}
}