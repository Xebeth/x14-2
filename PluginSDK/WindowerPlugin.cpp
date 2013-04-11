/**************************************************************************
	created		:	2013-04-11
	filename	: 	WindowerPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Windower plugin base class
**************************************************************************/
#include "stdafx.h"

#include "WindowerPlugin.h"

namespace Windower
{
	/*! \brief WindowerPlugin constructor
		\param[in] pServices_in : a pointer to the plugin services
	*/
	WindowerPlugin::WindowerPlugin(PluginFramework::IPluginServices *pServices_in)
		: PluginFramework::IPlugin(pServices_in) {}

	/*! \brief Update method to be overloaded by plugins
				This method will be called in the main loop of the windower engine
		\return true if any computation was performed; false otherwise
	*/
	bool WindowerPlugin::Update() { return false; }
}