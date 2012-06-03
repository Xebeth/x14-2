/**************************************************************************
	created		:	2011-10-30
	filename	: 	ModuleService.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Module service
**************************************************************************/
#ifndef __MODULE_SERVICE_H__
#define __MODULE_SERVICE_H__

typedef std::set<PluginFramework::PluginUUID> CompatiblePlugins;
typedef std::set<PluginFramework::IPlugin*> PluginSet;

namespace Windower
{
	//! \brief Module service
	class ModuleService
	{
	public:
		explicit ModuleService(bool InvokePermission_in);

		/*! \brief Checks if service can be invoked
			\return true if the service can be invoked; false otherwise
		*/
		bool CanInvoke() { return m_bInvokePermission; }
		/*! \brief Checks if the set of subscribers is empty
			\return true if the set is empty; false otherwise
		*/
		bool IsEmpty() { return m_Subscribers.empty(); }
		/*! \brief Retrieves the plugins subscribing to the service 
			\return the plugins subscribing to the service 
		*/
		const PluginSet& GetSubscribers() const { return m_Subscribers; }
		/*! \brief Adds a subscriber to the set
			\param[in] pSubscriber_in : the new subscriber
		*/
		void AddSubscriber(PluginFramework::IPlugin *pSubscriber_in)
		{ m_Subscribers.insert(pSubscriber_in); }
		/*! \brief Removes the subscriber at the specified position
			\param[in,out] Iter_in_out : the position of the subscriber to remove
		*/
		void RemoveSubscriber(PluginSet::const_iterator &Iter_in_out)
		{ Iter_in_out = m_Subscribers.erase(Iter_in_out); }

		void RemoveSubscriber(PluginFramework::IPlugin *pSubscriber_in);

	private:
		//! flag specifying if the service is available for invocation
		bool m_bInvokePermission;
		//! a set of plugins subscribing to a service
		PluginSet m_Subscribers;
	};
}

#endif//__MODULE_SERVICE_H__