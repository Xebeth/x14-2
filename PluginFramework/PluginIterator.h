/**************************************************************************
	created		:	2011-05-29
	filename	: 	PluginIterator.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin iterator
**************************************************************************/
#ifndef __PLUGIN_ITERATOR_H__
#define __PLUGIN_ITERATOR_H__

#ifndef _PLUGIN_FRAMEWORK_SDK_INCLUDE_
	#error Please include the global header file 'PluginFramework.h'
#endif

namespace PluginFramework
{
	/*! \brief Plugin iterator */
	class PluginIterator : public FileIterator
	{
	public:
		PluginIterator(const string_t &Directory_in, PluginManager &Manager_in,
					   DWORD CompatibilityFlags_in = 0xFFFFFFFF,
					   const TCHAR *pWildcard_in = _T("*.dll"));

		virtual bool IsValid();
	protected:
		//! compatibility flags
		DWORD m_CompatibilityFlags;
		//! a reference to the plugin manager
		PluginManager	&m_PluginManager;
		//! function pointer on the query function exposed by the plugins
		fnQuery			*m_pQueryFunc;
	};
}

#endif//__PLUGIN_ITERATOR_H__