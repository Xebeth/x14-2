/**************************************************************************
	created		:	2011-10-30
	filename	: 	ModuleService.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Module service
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>

#include "ModuleService.h"

namespace Windower
{
	/*! \brief ModuleService constructor
		\param[in] InvokePermission_in : invoke permission
	*/
	ModuleService::ModuleService(bool InvokePermission_in)
		: m_bInvokePermission(InvokePermission_in) {}

	/*! \brief Removes the specified subscriber
		\param[in] pSubscriber_in : the subscriber to remove
	*/
	void ModuleService::RemoveSubscriber(PluginFramework::IPlugin *pSubscriber_in)
	{
		size_t Size = m_Subscribers.size();

		if (Size > 0)
		{
			if (Size == 1 && *m_Subscribers.begin() == pSubscriber_in)
			{
				m_Subscribers.clear();
			}
			else
			{
				PluginSet::const_iterator Iter = m_Subscribers.find(pSubscriber_in);

				if (Iter != m_Subscribers.end())
					RemoveSubscriber(Iter);
			}
		}
	}
}