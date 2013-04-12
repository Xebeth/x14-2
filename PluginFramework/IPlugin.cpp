/**************************************************************************
	created		:	2011-11-06
	filename	: 	IPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin interface
**************************************************************************/
#include "stdafx.h"

#include "FrameworkVersion.h"
#include "VersionInfo.h"
#include "PluginInfo.h"
#include "IPlugin.h"

#include "IPluginServices.h"

namespace PluginFramework
{
	IPluginServices * IPlugin::m_pServices = NULL;

	IPlugin::IPlugin(IPluginServices *pServices_in)
		: m_pRegisterParams(NULL)
	{
		m_pServices = pServices_in;
	}

	IPlugin::~IPlugin() {}

	RegisterParams::RegisterParams()
		: CreateFunc(NULL), DestroyFunc(NULL), QueryFunc(NULL), ConfigureFunc(NULL) {}

	bool IPlugin::Initialize(RegisterParams &RegisterParams_out, fnCreate pfnCreateFunc_in, 
							 fnDestroy pfnDestroyFunc_in, fnQuery pfnQueryFunc_in,
							 fnConfigure pfnConfigureFunc_in)
	{
		if (pfnCreateFunc_in != NULL && pfnDestroyFunc_in != NULL && pfnQueryFunc_in != NULL)
		{
			RegisterParams_out.ConfigureFunc = pfnConfigureFunc_in;
			RegisterParams_out.DestroyFunc = pfnDestroyFunc_in;
			RegisterParams_out.CreateFunc = pfnCreateFunc_in;
			RegisterParams_out.QueryFunc = pfnQueryFunc_in;

			pfnQueryFunc_in(RegisterParams_out.Info);
			Query(RegisterParams_out.Info);

			return true;
		}

		return false;
	}

	bool IPlugin::Configure(const LPVOID pUserData_in)
	{
		if (IsConfigurable() == false)
		{
			MessageBox(NULL, _T("This plugin has no configuration."),
					   m_PluginInfo.GetName().c_str(),
					   MB_OK | MB_ICONINFORMATION);

			return false;
		}
		else
			return m_pRegisterParams->ConfigureFunc(this, pUserData_in);
	}

	string_t IPlugin::ToString() const { return m_PluginInfo.ToString(); }

	bool IPlugin::SubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in)
	{
		return (m_pServices != NULL && m_pServices->SubscribeService(ModuleName_in, ServiceName_in, this));
	}

	bool IPlugin::UnsubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in)
	{
		return (m_pServices != NULL && m_pServices->UnsubscribeService(ModuleName_in, ServiceName_in, this));
	}

	void IPlugin::OnCreate()
	{ Subscribe(); }
	void IPlugin::OnDestroy()
	{ Unsubscribe(); }

	void IPlugin::Query(PluginFramework::PluginInfo& PluginInfo_out)
	{
		PluginInfo_out.m_FrameworkVersion.FromString(__PLUGIN_FRAMEWORK_VERSION__);
		PluginInfo_out.m_Initialized = true;

		CleanupName(PluginInfo_out.m_Name);
	}

	void IPlugin::CleanupName( string_t Name_in_out )
	{
		string_t::size_type Pos, Offset = 0;

		Pos = Name_in_out.find_first_of('|', Offset);

		while(Pos != string_t::npos)
		{
			Name_in_out.erase(Pos, 1);

			Offset = Pos + 1;
			Pos = Name_in_out.find_first_of('|', Offset);
		}
	}

	bool IPlugin::InvokeService(const string_t &ModuleName_in,
								const string_t &ServiceName_in,
								const ServiceParam &Params_in)
	{
		return (m_pServices != NULL && m_pServices->InvokeService(ModuleName_in, ServiceName_in, Params_in));
	}

	const TCHAR* IPlugin::GetConfigFile()
	{ return m_pServices->GetConfigFile(); }

	const VersionInfo& IPlugin::GetFrameworkVersion() const
	{ return m_PluginInfo.m_FrameworkVersion; }

	const VersionInfo& IPlugin::GetVersion() const
	{ return m_PluginInfo.m_Version; }

	const string_t& IPlugin::GetUpdateURL() const
	{ return m_PluginInfo.m_UpdateURL; }

	const string_t& IPlugin::GetDLLPath() const
	{ return m_PluginInfo.m_DLLPath; }

	const string_t& IPlugin::GetAuthor() const
	{ return m_PluginInfo.m_Author; }

	const StringUtils::UUID& IPlugin::GetUUID() const
	{ return m_PluginInfo.m_PluginIdentifier; }

	const string_t& IPlugin::GetName() const
	{ return m_PluginInfo.m_Name; }

	const string_t& IPlugin::GetDesc() const
	{ return m_PluginInfo.m_Description; }

	DWORD IPlugin::GetCompatibilityFlags() const
	{ return m_PluginInfo.m_CompatibilityFlags; }

	bool IPlugin::IsInitialized() const
	{ return m_PluginInfo.m_Initialized; }

	bool IPlugin::IsConfigurable() const
	{ return (m_pRegisterParams != NULL && m_pRegisterParams->ConfigureFunc != NULL); }

	HMODULE IPlugin::GetHandle() const
	{ return m_PluginInfo.m_hHandle; }

	void IPlugin::SetUpdateURL(const string_t &UpdateURL_in)
	{ m_PluginInfo.m_UpdateURL = UpdateURL_in; }

	void IPlugin::SetDesc(const string_t &Description_in)
	{ m_PluginInfo.m_Description = Description_in; }

	void IPlugin::SetAuthor(const string_t &Author_in)
	{ m_PluginInfo.m_Author = Author_in; }

	void IPlugin::SetVersion(const string_t &Version_in)
	{ m_PluginInfo.m_Version.FromString(Version_in.c_str()); }

	void IPlugin::SetName(const string_t &Name_in)
	{ m_PluginInfo.m_Name = Name_in; }

	bool IPlugin::Unsubscribe()
	{ return true; }
	bool IPlugin::Subscribe()
	{ return true; }
}