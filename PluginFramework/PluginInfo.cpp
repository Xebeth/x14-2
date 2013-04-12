/**************************************************************************
	created		:	2011-11-06
	filename	: 	PluginInfo.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin information class
**************************************************************************/
#include "stdafx.h"

#include "VersionInfo.h"
#include "PluginInfo.h"

namespace PluginFramework
{
	PluginInfo::PluginInfo() : m_Initialized(false), m_hHandle(NULL),
		m_CompatibilityFlags(0U) {}

	string_t PluginInfo::ToString() const
	{
		return format(_T("Plugin '%s v%s':\n")
					  _T("  Author:      %s\n")
					  _T("  Description: %s\n"),
					  m_Name.c_str(), m_Version.ToString().c_str(),
					  m_Author.c_str(), m_Description.c_str());
	}

	const string_t& PluginInfo::GetDLLPath() const
	{ return m_DLLPath; }

	string_t PluginInfo::GetFrameworkVersion() const
	{ return m_FrameworkVersion.ToString(); }

	const string_t& PluginInfo::GetUpdateURL() const
	{ return m_UpdateURL; }	
	void PluginInfo::SetUpdateURL(const string_t &UpdateURL_in)
	{ m_UpdateURL = UpdateURL_in; }

	DWORD PluginInfo::GetCompatibilityFlags() const
	{ return m_CompatibilityFlags; }
	void PluginInfo::SetCompatibilityFlags(DWORD CompatibilityFlags_in)
	{ m_CompatibilityFlags = CompatibilityFlags_in; }

	const string_t& PluginInfo::GetName() const
	{ return m_Name; }
	void PluginInfo::SetName(const string_t &Name_in)
	{ m_Name = Name_in; }

	string_t PluginInfo::GetVersion() const
	{ return m_Version.ToString(); }
	void PluginInfo::SetVersion(const string_t &Version_in)
	{ m_Version.FromString(Version_in.c_str()); }

	string_t PluginInfo::GetIndentifier() const
	{ return m_PluginIdentifier.ToString(); }
	void PluginInfo::SetIdentifier(const string_t &UUID_in)
	{ m_PluginIdentifier.FromString(UUID_in.c_str()); }

	const string_t& PluginInfo::GetAuthor() const
	{ return m_Author; }
	void PluginInfo::SetAuthor(const string_t &Author_in)
	{ m_Author = Author_in; }

	const string_t& PluginInfo::GetDesc() const
	{ return m_Description; }
	void PluginInfo::SetDesc(const string_t &Description_in)
	{ m_Description = Description_in; }
}