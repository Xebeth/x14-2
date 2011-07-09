/**************************************************************************
	created		:	2011-07-03
	filename	: 	PluginsServices.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <hash_map>

#include <PluginFramework.h>
#include <HookEngine.h>

#include "PluginsServices.h"
#include "PluginManager.h"
#include "ICoreModule.h"
#include "HookManager.h"

namespace Windower
{
	bool PluginServices::SubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
										  PluginFramework::IPlugin* pPlugin_in) const
	{
		if (pPlugin_in != NULL)
		{
			CoreModules::const_iterator Iter = m_Modules.find(ModuleName_in);

			if (Iter != m_Modules.end() && Iter->second != NULL)
				return Iter->second->Subscribe(ServiceName_in, static_cast<PluginFramework::IPlugin*>(pPlugin_in));
		}

		return false;
	}

	bool PluginServices::UnsubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
											PluginFramework::IPlugin* pPlugin_in) const
	{
		if (pPlugin_in != NULL)
		{
			CoreModules::const_iterator Iter = m_Modules.find(ModuleName_in);

			if (Iter != m_Modules.end() && Iter->second != NULL)
				return Iter->second->Unsubscribe(ServiceName_in, static_cast<PluginFramework::IPlugin*>(pPlugin_in));
		}

		return false;
	}

	bool PluginServices::InvokeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
									   const PluginFramework::ServiceParam &Params_in,
									   PluginFramework::ServiceParam &Results_out) const
	{
		CoreModules::const_iterator Iter = m_Modules.find(ModuleName_in);

		if (Iter != m_Modules.end() && Iter->second != NULL)
			return Iter->second->Invoke(ServiceName_in, Params_in, Results_out);

		return false;
	}
}