/**************************************************************************
	created		:	2010-10-23
	filename	: 	ICoreModule.cpp
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Base class for the windower modules
**************************************************************************/
#include "stdafx.h"

#include "ICoreModule.h"

namespace Windower
{
	ICoreModule::~ICoreModule() {}

	void ICoreModule::SetPluginManager(PluginFramework::PluginManager &Manager_in_out)
	{ m_pPluginManager = &Manager_in_out; }
}