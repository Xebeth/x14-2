/**************************************************************************
	created		:	2011-07-03
	filename	: 	WindowerCore.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <NonCopyable.h>
#include <HookEngine.h>

#include "BaseEngine.h"
#include "PluginEngine.h"
#include "WindowerEngine.h"

#include "ICoreModule.h"
#include "WindowerCore.h"

namespace Windower
{
	void WindowerCore::RegisterService(const string_t &ServiceName_in, bool InvokePermission_in)
	{
		ModuleServices::const_iterator Iter = m_Services.find(ServiceName_in);

		// the service doesn't exist
		if (Iter == m_Services.end())
			m_Services[ServiceName_in] = ModuleService(InvokePermission_in, PluginSet());
	}

	bool WindowerCore::IsPluginCompatible(PluginFramework::IPlugin* pPlugin_in)
	{
		return (pPlugin_in != NULL && m_CompatiblePlugins.find(pPlugin_in->GetInfo().PluginIdentifier) != m_CompatiblePlugins.end());
	}

	void WindowerCore::UnsubscribeAll(PluginFramework::IPlugin* pPlugin_in)
	{
		ModuleServices::const_iterator Iter;

		for (Iter = m_Services.begin(); Iter != m_Services.end(); ++Iter)
			Unsubscribe(Iter->first, pPlugin_in);
	}

	bool WindowerCore::Subscribe(const string_t &ServiceName_in, PluginFramework::IPlugin* pPlugin_in)
	{
		if (IsPluginCompatible(pPlugin_in))
		{
			ModuleServices::iterator Iter = m_Services.find(ServiceName_in);

			if (Iter != m_Services.end())
			{
				PluginSet::const_iterator SubsIter = Iter->second.Subscribers.find(pPlugin_in);

				if (SubsIter == Iter->second.Subscribers.end())
				{
					Iter->second.Subscribers.insert(pPlugin_in);
					OnSubscribe(ServiceName_in, Iter->second.Subscribers);
				}

				return true;
			}
		}

		return false;
	}

	bool WindowerCore::Unsubscribe(const string_t &ServiceName_in, PluginFramework::IPlugin* pPlugin_in)
	{
		ModuleServices::iterator Iter = m_Services.find(ServiceName_in);

		if (Iter != m_Services.end())
		{
			PluginSet::const_iterator SubsIter = Iter->second.Subscribers.find(pPlugin_in);

			if (SubsIter != Iter->second.Subscribers.end())
			{
				Iter->second.Subscribers.erase(SubsIter);
				OnUnsubscribe(ServiceName_in, Iter->second.Subscribers);
			}

			return true;
		}

		return false;
	}
}
