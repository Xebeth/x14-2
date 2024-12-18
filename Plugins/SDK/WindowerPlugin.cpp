/**************************************************************************
	created		:	2013-04-11
	filename	: 	WindowerPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Windower plugin base class
**************************************************************************/
#include "stdafx.h"
#include <afxdlgs.h>

#include "ConfigurablePlugin.h"
#include "PluginPropertyPage.h"
#include "WindowerPlugin.h"

namespace Windower
{
	/*! \brief WindowerPlugin constructor
		\param[in] pServices_in : a pointer to the plugin services
	*/
	WindowerPlugin::WindowerPlugin(PluginFramework::IPluginServices *pServices_in)
		: ConfigurablePlugin(pServices_in) {}

	/*! \brief Update method to be overloaded by plugins
				This method will be called in the main loop of the windower engine
		\return true if any computation was performed; false otherwise
	*/
	bool WindowerPlugin::Update() { return false; }

	//! Callback function invoked when a new instance of the plugin has been created
	void WindowerPlugin::OnCreate()
	{
		ConfigurablePlugin::OnCreate();
		RegisterCommands();
	}

	//! Callback function invoked when a new instance of the plugin is about to be destroyed
	void WindowerPlugin::OnDestroy()
	{
		UnregisterCommands();
		ConfigurablePlugin::OnDestroy();
	}

	/*! \brief Registers the commands of the plugin with the command dispatcher
		\return true if all the commands were registered successfully; false otherwise
	*/
	bool WindowerPlugin::RegisterCommands() { return true; }
	
	/*! \brief Unregisters the commands of the plugin with the command dispatcher
		\return true if all the commands were unregistered successfully; false otherwise
	*/
	bool WindowerPlugin::UnregisterCommands() { return true; }

	/*! \brief Retrieves the property page used to configure the plugin
		\return a pointer to the property page of the plugin
	*/
	PluginPropertyPage* WindowerPlugin::GetPropertyPage()
	{
		return nullptr;
	}

	//! \brief Callback function invoked when the settings have changed
	void WindowerPlugin::OnSettingsChanged() {}
}