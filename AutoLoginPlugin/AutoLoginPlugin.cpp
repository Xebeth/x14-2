/**************************************************************************
	created		:	2011-06-04
	filename	: 	AutoLoginPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Tell detect plugin
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <SettingsManager.h>

#include "AutoLoginSettings.h"
#include "WindowerCommand.h"
#include "CryptUtils.h"
#include "HTMLEventSink.h"
#include "AutoLoginPlugin.h"
#include "AutoLogin.h"

const PluginFramework::IPluginServices* PluginFramework::IPlugin::m_pPluginServices = NULL;

namespace Windower
{	
	AutoLoginSettings * AutoLoginPlugin::m_pSettings = NULL;

	//! \brief AutoLoginPlugin default constructor
	AutoLoginPlugin::AutoLoginPlugin()
	{
		m_pSettings = new AutoLoginSettings(_T("autologin.ini"));
	}
	
	//! \brief AutoLoginPlugin destructor
	AutoLoginPlugin::~AutoLoginPlugin()
	{
		if (m_pSettings != NULL)
		{
			delete m_pSettings;
			m_pSettings = NULL;
		}
	}

	/*! \brief Creates an instance of AutoLoginPlugin
		\return a pointer to the new AutoLoginPlugin instance
	*/
	void* AutoLoginPlugin::Create()
	{
		AutoLoginPlugin *pNewInst = new AutoLoginPlugin;
		AutoLoginPlugin::Query(pNewInst->m_BasePluginInfo);

		CallerParam Caller("AutoLoginPlugin", pNewInst);
		const char *pParamName = "hwnd";
		CommandParameters Params;

		Params[pParamName].Name = pParamName;
		Params[pParamName].Type = COMMAND_PARAM_TYPE_INTEGER;
		Params[pParamName].Value = "0";
		Params[pParamName].Description = "fills the password field automatically during login ";
		WindowerCommand Command(PLUGIN_REGKEY, "autologin::startthread", "starts the thread to auto login", Caller, AutoLoginThread, Params, 1, 1);
		PluginFramework::ServiceParam InvokeArg(_T("WindowerCommand"), &Command);

		m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("RegisterCommand"), InvokeArg, PluginFramework::ServiceParam());
		
		return pNewInst;
	}

	int AutoLoginPlugin::AutoLoginThread(const WindowerCommand *pCommand_in)
	{
		if (m_pSettings != NULL)
		{
			if (pCommand_in != NULL && pCommand_in->Caller.DataType.compare("AutoLoginPlugin") == 0)
			{
				AutoLoginPlugin *pTimestamp = reinterpret_cast<AutoLoginPlugin*>(pCommand_in->Caller.pData);
				const WindowerCommandParam *pParam = pCommand_in->GetParameter("hwnd");

				m_pSettings->SetParentWnd((HWND)pParam->GetIntegerValue());
				CreateThread(NULL, 0, ::AutoLoginThread, (LPVOID)m_pSettings, 0, NULL);	

				return DISPATCHER_RESULT_SUCCESS;
			}
		}

		return DISPATCHER_RESULT_INVALID_CALL;
	}

	/*! \brief Destroys an instance of AutoLoginPlugin
		\param[in] pInstance_in : an instance of AutoLoginPlugin
	*/
	void AutoLoginPlugin::Destroy(void *pInstance_in)
	{
		if (pInstance_in != NULL)
		{
			delete (AutoLoginPlugin *)pInstance_in;
			pInstance_in = NULL;
		}
	}

	/*! \brief Fills a PluginInfo structure with the plugin information
		\param[out] Info_out : a PluginInfo structure
	*/
	void AutoLoginPlugin::Query(PluginInfo& Info_out)
	{
		Info_out.Author = _T("Xebeth`");
		Info_out.Name = _T("AutoLogin");
		Info_out.PluginVersion.FromString(_T("1.0.0"));
		Info_out.FrameworkVersion.FromString(_T("1.0.0"));
		Info_out.Descritpion = _T("This plugin will automatically fill the password field during login");
		Info_out.PluginIdentifier.FromString(_T("9D0E9E80-A5CB-11E0-8264-0800200C9A66"));
	}
}

using Windower::AutoLoginPlugin;

extern "C" PLUGIN_API void QueryPlugin(PluginInfo &Info_out)
{
	AutoLoginPlugin::Query(Info_out);
}

extern "C" PLUGIN_API int TerminatePlugin()
{
	return 0;
}

extern "C" PLUGIN_API RegisterParams* InitPlugin(const PluginFramework::IPluginServices *pServicesParams_in)
{
	if (pServicesParams_in != NULL)
	{
		RegisterParams *pParams = new RegisterParams;

		AutoLoginPlugin::Query(pParams->Info);

		pParams->QueryFunc = AutoLoginPlugin::Query;
		pParams->CreateFunc = AutoLoginPlugin::Create;
		pParams->DestroyFunc = AutoLoginPlugin::Destroy;

		AutoLoginPlugin::SetPluginServices(pServicesParams_in);

		return pParams;
	}

	return NULL;
}