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
		: IPlayerDataPlugin(pServices_in), m_Distance(0.f),
		  m_pPlayerX(NULL), m_pPlayerY(NULL), m_pPlayerZ(NULL),
		  m_pTargetX(NULL), m_pTargetY(NULL), m_pTargetZ(NULL){}

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

	void DistancePlugin::OnPlayerPtrChange(const TargetData *pPlayerData_in)
	{
		if (pPlayerData_in != NULL)
		{
			m_pPlayerX = &pPlayerData_in->PosX;
			m_pPlayerY = &pPlayerData_in->PosY;
			m_pPlayerZ = &pPlayerData_in->PosZ;
		}
		else
		{
			m_pPlayerX = m_pPlayerY = m_pPlayerZ;
		}
	}

	void DistancePlugin::OnTargetPtrChange(const TargetData *pTargetData_in)
	{
		if (pTargetData_in != NULL)
		{
			m_pTargetName = const_cast<char*>(pTargetData_in->Name);
			m_pTargetX = &pTargetData_in->PosX;
			m_pTargetY = &pTargetData_in->PosY;
			m_pTargetZ = &pTargetData_in->PosZ;			
		}
		else
		{
			m_pTargetX = m_pTargetY = m_pTargetZ;
			m_pTargetName = NULL;
		}
	}

	bool DistancePlugin::Update()
	{
		if (m_pPlayerX != NULL && m_pPlayerY != NULL && m_pPlayerZ != NULL
		 && m_pTargetX != NULL && m_pTargetY != NULL && m_pTargetZ != NULL)
		{
			float dX = *m_pPlayerX - *m_pTargetX;
			float dY = *m_pPlayerY - *m_pTargetY;
			float dZ = *m_pPlayerZ - *m_pTargetZ;

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