/**************************************************************************
	created		:	2011-06-04
	filename	: 	DistancePlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	m_LabelText plugin
**************************************************************************/
#include "stdafx.h"

#include "DistancePlugin.h"
#include "version.h"

namespace Windower
{
	//! \brief DistancePlugin constructor
	DistancePlugin::DistancePlugin(PluginFramework::IPluginServices *pServices_in)
		: IPlayerDataPlugin(pServices_in), m_Distance(0.f), m_pTextLabel(NULL),
		  m_pPlayerData(NULL), m_pPlayerTarget(NULL) {}

	DistancePlugin::~DistancePlugin()
	{
		if (m_pTextLabel != NULL)
		{
			LabelServiceParam LabelParam(&m_pTextLabel);
			PluginFramework::ServiceParam Param(_T("LabelServiceParam"), &LabelParam);

			if (InvokeService(_T(GRAPHICS_MODULE), _T(TEXT_LABEL_SERVICE), Param) == false)
				m_pTextLabel = NULL;
		}
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
		PluginInfo_out.SetAuthor(_T("Xebeth`"));
		PluginInfo_out.SetName(_T("Distance"));
	}

	void DistancePlugin::OnTargetPtrChange(const TargetData **pPlayerData_in, const TargetData **pTargetData_in)
	{
		m_pPlayerTarget = pTargetData_in;
		m_pPlayerData = pPlayerData_in;

		if (m_pPlayerTarget != NULL && *m_pPlayerTarget != NULL && (*m_pPlayerTarget)->Name != NULL)
		{
			std::string TargetName = (*m_pPlayerTarget)->Name;

			// convert only once to UTF-8 for display
			convert_utf8(TargetName, m_TargetName);

			if (m_pTextLabel == NULL)
			{
				LabelServiceParam LabelParam(&m_pTextLabel, _T("Distance##label"), 5L, 5L, 500UL, 16UL, _T("Arial"));
				PluginFramework::ServiceParam Param(_T("LabelServiceParam"), &LabelParam);

				if (InvokeService(_T(GRAPHICS_MODULE), _T(TEXT_LABEL_SERVICE), Param) == false)
					m_pTextLabel = NULL;
			}
		}
	}

	bool DistancePlugin::Update()
	{
		if (m_pPlayerTarget != NULL && *m_pPlayerTarget != NULL
		 && m_pPlayerData != NULL && *m_pPlayerData != NULL)
		{
			float dX = (*m_pPlayerData)->PosX - (*m_pPlayerTarget)->PosX;
			float dY = (*m_pPlayerData)->PosY - (*m_pPlayerTarget)->PosY;
			float dZ = (*m_pPlayerData)->PosZ - (*m_pPlayerTarget)->PosZ;

			m_Distance = sqrtf(dX * dX + dY * dY + dZ * dZ);

			if (m_pTextLabel != NULL)
			{
#ifdef _DEBUG
				format(m_LabelText, _T("%s (0x%08X) => %.2f"), m_TargetName.c_str(), *m_pPlayerData, m_Distance);
#else
				format(m_LabelText, _T("%s => %.2f"), m_TargetName.c_str(), m_Distance);
#endif // _DEBUG
				m_pTextLabel->SetTitleText(m_LabelText);
				m_pTextLabel->SetVisibile(true);
			}

			return true;
		}
		else if (m_pTextLabel != NULL)
		{
			m_pTextLabel->SetVisibile(false);
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