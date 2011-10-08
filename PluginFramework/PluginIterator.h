/**************************************************************************
	created		:	2011-05-29
	filename	: 	PluginIterator.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin iterator
**************************************************************************/
#ifndef __PLUGIN_ITERATOR_H__
#define __PLUGIN_ITERATOR_H__

namespace PluginFramework
{
	/*! \brief Plugin iterator */
	class PluginIterator : public FileIterator
	{
	public:
		PluginIterator(const string_t &Directory_in, PluginManager &Manager_in,
					   const TCHAR *pWildcard_in = _T("*.dll"));

		virtual bool IsValid();
	protected:
		PluginManager	&m_PluginManager;
		fnQuery			*m_pQueryFunc;
	};
}

#endif//__PLUGIN_ITERATOR_H__