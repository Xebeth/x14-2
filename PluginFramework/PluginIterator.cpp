/**************************************************************************
	created		:	2011-05-29
	filename	: 	PluginIterator.cpp
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin iterator
**************************************************************************/
#include "stdafx.h"

#include "VersionInfo.h"
#include "IPlugin.h"

#include "IPluginServices.h"
#include "PluginManager.h"
#include "FileIterator.h"
#include "PluginIterator.h"

namespace PluginFramework
{
	/*! \brief PluginIterator constructor
		\param[in] Directory_in : the directory in which to look for files
		\param[in] Manager_in : a reference to the plugin manager
		\param[in] pWildcard_in : the wildcard used to select files (e.g: *.dll)
	*/
	PluginIterator::PluginIterator(const string_t &Directory_in, PluginManager &Manager_in, 
								   DWORD CompatibilityFlags_in, const TCHAR *pWildcard_in)
		: FileIterator(Directory_in, pWildcard_in), m_PluginManager(Manager_in),
		  m_CompatibilityFlags(CompatibilityFlags_in) {}

	/*! \brief Checks if a plugin is valid through the challenge/response mechanism of the manager
		\return true if the plugin is valid; false otherwise
	*/
	bool PluginIterator::IsValid()
	{
		string_t ModulePath;

		format(ModulePath, _T("%s%s"), m_Directory.c_str(), Current());

		return (FileIterator::IsValid() && m_PluginManager.IsPluginValid(ModulePath.c_str(), m_CompatibilityFlags));
	}
}