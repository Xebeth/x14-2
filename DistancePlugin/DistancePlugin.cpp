/**************************************************************************
	created		:	2011-06-04
	filename	: 	DistancePlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Distance plugin
**************************************************************************/
#include "stdafx.h"

#include "DistancePlugin.h"
#include "version.h"

namespace Windower
{
	//! \brief DistancePlugin constructor
	DistancePlugin::DistancePlugin(PluginFramework::IPluginServices *pServices_in)
		: IPlayerDataPlugin(pServices_in), m_Distance(0.f)
	{
		memset(&m_TargetPos, 0, sizeof(m_TargetPos));
		memset(&m_PlayerPos, 0, sizeof(m_PlayerPos));
	}

	/*! \brief Creates an instance of DistancePlugin
		\param[in] pServices_in : a pointer to the plugin services
		\return a pointer to the new DistancePlugin instance
	*/
	PluginFramework::IPlugin* DistancePlugin::Create(PluginFramework::IPluginServices *pServices_in)
	{
		return new DistancePlugin(pServices_in);
	}

	/*! \brief Destroys an instance of DistancePlugin
		\param[in] pInstance_in : an instance of DistancePlugin
	*/
	void DistancePlugin::Destroy(PluginFramework::IPlugin *pInstance_in)
	{
		if (pInstance_in != NULL)
		{
			delete pInstance_in;
			pInstance_in = NULL;
		}
	}

	/*! \brief Fills a VersionInfo structure with the plugin information
		\param[out] PluginInfo_out : a VersionInfo structure
	*/
	void DistancePlugin::Query(PluginFramework::PluginInfo& PluginInfo_out)
	{
		PluginInfo_out.SetDesc(_T("This plugin will display the distance to the player target"));
		PluginInfo_out.SetIdentifier(_T("F4F02060-9ED0-11E2-9E96-0800200C9A66"));
		PluginInfo_out.SetCompatibilityFlags(PLUGIN_COMPATIBILITY_WINDOWER);
		PluginInfo_out.SetVersion(PLUGIN_VERSION);
		PluginInfo_out.SetName(_T("Distance"));
		PluginInfo_out.SetAuthor(_T("Xebeth`"));
	}

	void DistancePlugin::OnPlayerPtrChange(const TargetPos &PlayerData_in)
	{
		m_PlayerPos = PlayerData_in;
	}

	void DistancePlugin::OnTargetPtrChange(const TargetPos &TargetData_in)
	{
		m_TargetPos = TargetData_in;
	}

	bool DistancePlugin::Update()
	{
		if (m_TargetPos.pPosX != NULL && m_TargetPos.pPosY != NULL && m_TargetPos.pPosZ != NULL
		 && m_TargetPos.pPosX != NULL && m_TargetPos.pPosY != NULL && m_TargetPos.pPosZ != NULL)
		{
			float dX = *m_PlayerPos.pPosX - *m_TargetPos.pPosX;
			float dY = *m_PlayerPos.pPosY - *m_TargetPos.pPosY;
			float dZ = *m_PlayerPos.pPosZ - *m_TargetPos.pPosZ;

			m_Distance = sqrtf(dX * dX + dY * dY + dZ * dZ);

			return true;
		}

		return false;
	}
}

using Windower::DistancePlugin;

/*! \brief Function exposed by the plugin DLL to initialize the plugin object
	\param[out] RegisterParams_out : Registration structure to be able to use the plugin
	\return true if the initialization succeeded; false otherwise
*/
extern "C" PLUGIN_API bool InitPlugin(PluginFramework::RegisterParams &RegisterParams_out)
{
	return PluginFramework::IPlugin::Initialize(RegisterParams_out, DistancePlugin::Create,
												DistancePlugin::Destroy, DistancePlugin::Query);
}