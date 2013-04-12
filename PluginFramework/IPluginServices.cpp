/**************************************************************************
	created		:	2011-07-03
	filename	: 	IPluginServices.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin services interface
**************************************************************************/
#include "stdafx.h"

#include "VersionInfo.h"
#include "IPluginServices.h"

namespace PluginFramework
{
	ServiceParam::ServiceParam()
		: DataType(_T("NULL")), pData(NULL) {}

	ServiceParam::ServiceParam(string_t DataType_in, LPVOID pData_in)
		: DataType(DataType_in), pData(pData_in) {}

	IPluginServices::IPluginServices(const VersionInfo &Version_in)
		: m_Version(Version_in) {}

	IPluginServices::~IPluginServices() {}

	const VersionInfo& IPluginServices::GetVersion() const
	{ return m_Version; }
}