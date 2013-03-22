/**************************************************************************
	created		:	2013-03-12
	filename	: 	ModuleService.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Module service class
**************************************************************************/
#ifndef __MODULE_SERVICE_H__
#define __MODULE_SERVICE_H__

#include <NonCopyable.h>
#include <hash_set>
#include <set>

//! the hooks of a service
typedef stdext::hash_map<std::string, LPVOID> HookPointers;

namespace Windower
{
	//! a set of UUID
	typedef std::set<PluginFramework::PluginUUID> CompatiblePlugins;
	//! a set of plugins
	typedef std::set<PluginFramework::IPlugin*> PluginSet;

	//! Calling context base class
	class BaseContext : public NonCopyable
	{
	public:
		BaseContext(LPVOID pTrampoline_in, PluginSet &Subscribers_in)
			: m_Subscribers(Subscribers_in), m_pTrampoline(pTrampoline_in) {}

		//! pointer to the original function of the hook
		LPVOID m_pTrampoline;
		//! subscribers to the service
		PluginSet &m_Subscribers;
	};

	//! Module service
	class ModuleService : public NonCopyable
	{
	public:
		ModuleService(const string_t& Name_in, const HookPointers &Hooks_in, bool InvokePermission_in = false);

		bool IsPluginCompatible(PluginFramework::IPlugin* pPlugin_in);
		bool RemoveSubscriber(PluginFramework::IPlugin *pPlugin_in);
		bool AddSubscriber(PluginFramework::IPlugin *pPlugin_in);

		/*! \brief Checks if the service can be invoked
			\return true if the service can be invoked; false otherwise
		*/
		bool CanInvoke() const { return m_InvokePermission; }
		/*! \brief Retrieves the name of the service
			\return the name of the service
		*/
		const string_t& GetName() const { return m_ServiceName; }
		/*! \brief Retrieves the subscribers to the service
			\return the set of plugins subscribing to the service
		*/
		const PluginSet& GetSubscribers() const { return m_Subscribers; }
		/*! \brief Retrieves the hooks associated with the service
			\return the hooks associated with the service
		*/
		HookPointers& GetHooks() { return m_ServiceHooks; }

		//! Creates the calling context for the service
		virtual void CreateContext() {}
		//! Destroys the calling context for the service
		virtual void DestroyContext() {}

	protected:
		//! the name of the service
		string_t m_ServiceName;
		//! flag specifying if the service can be invoked
		bool m_InvokePermission;
		//! the service subscribers
		PluginSet m_Subscribers;
		//! a set of compatible plugins
		CompatiblePlugins m_CompatiblePlugins;
		//! the hooks associated with the service
		HookPointers m_ServiceHooks;
	};

}

#endif//__MODULE_SERVICE_H__
