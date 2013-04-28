/**************************************************************************
	created		:	2013-03-12
	filename	: 	ModuleService.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Module service class
**************************************************************************/
#ifndef __MODULE_SERVICE_H__
#define __MODULE_SERVICE_H__

//! the hooks of a service
typedef stdext::hash_map<std::string, LPVOID> HookPointers;

namespace PluginFramework
{
	class ServiceParam;
}

namespace Windower
{
	//! Calling context base class
	class BaseContext : public NonCopyable
	{
	public:
		BaseContext(LPVOID pTrampoline_in, PluginFramework::PluginSet &Subscribers_in)
			: m_Subscribers(Subscribers_in), m_pTrampoline(pTrampoline_in) {}

		//! pointer to the original function of the hook
		LPVOID m_pTrampoline;
		//! subscribers to the service
		PluginFramework::PluginSet &m_Subscribers;
	};

	class BaseModuleService : public NonCopyable
	{
	public:
		BaseModuleService(const string_t& Name_in, bool InvokePermission_in = false)
			: m_ServiceName(Name_in), m_InvokePermission(InvokePermission_in) {}

		/*! \brief Checks if the service can be invoked
			\return true if the service can be invoked; false otherwise
		*/
		bool CanInvoke() const { return m_InvokePermission; }
		/*! \brief Retrieves the name of the service
			\return the name of the service
		*/
		const string_t& GetName() const { return m_ServiceName; }

		/*! \brief Checks if plugins can subscribe to this service
			\return true if plugins can subscribe; false otherwise
		*/
		virtual bool CanSubscribe() const { return false; }

		/*! \brief Invokes the service
			\param[in] Params[in] : user data
			\return true if the service was invoked successfully; false otherwise
		*/
		virtual bool Invoke(PluginFramework::ServiceParam &Params_in) { return CanInvoke(); }

	protected:
		//! the name of the service
		string_t m_ServiceName;
		//! flag specifying if the service can be invoked
		bool m_InvokePermission;
	};

	//! Module service
	class ModuleService : public BaseModuleService
	{
	public:
		ModuleService(const string_t& Name_in, bool InvokePermission_in = false);

		bool SetPointer(const std::string &HookName_in, LPVOID pPointer_in, bool Create_in = true);
		bool IsSubscriberKeyValid(PluginFramework::IPlugin* pPlugin_in);
		bool RemoveSubscriber(PluginFramework::IPlugin *pPlugin_in);
		bool AddSubscriber(PluginFramework::IPlugin *pPlugin_in);

		/*! \brief Checks if plugins can subscribe to this service
			\return true if plugins can subscribe; false otherwise
		*/
		bool CanSubscribe() const { return true; }

		/*! \brief Retrieves the subscribers to the service
			\return the set of plugins subscribing to the service
		*/
		const PluginFramework::PluginSet& GetSubscribers() const { return m_Subscribers; }

		/*! \brief Retrieves the hooks associated with the service
			\return the hooks associated with the service
		*/
		const HookPointers& GetHooks() { return m_ServiceHooks; }

		//! Creates the calling context for the service
		virtual void CreateContext() {}
		//! Destroys the calling context for the service
		virtual void DestroyContext() {}

	protected:
		//! the service subscribers
		PluginFramework::PluginSet m_Subscribers;
		//! a set of compatible plugins
		PluginFramework::CompatiblePlugins m_CompatiblePlugins;
		//! the hooks associated with the service
		HookPointers m_ServiceHooks;
	};

}

#endif//__MODULE_SERVICE_H__
