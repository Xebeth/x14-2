/**************************************************************************
	created		:	2011-06-04
	filename	: 	ICreateXmlNodePlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Alloc string plugin interface
**************************************************************************/
#ifndef __IALLOC_STRING_PLUGIN_H__
#define __IALLOC_STRING_PLUGIN_H__

#include <queue>

#include "IGameChatPlugin.h"

namespace Windower
{
	class ICreateXmlNodePlugin : public IGameChatPlugin
	{
	public:
		explicit ICreateXmlNodePlugin(const std::string &TargetText_in)
			: m_TargetText(TargetText_in), m_pTargetPtr(NULL) {}

		virtual ~ICreateXmlNodePlugin() { m_pTargetPtr = NULL; }

		virtual const char* OnCreateXmlNode(const char *pText_in, bool &Unsubscribe_out) =0;

	protected:
		std::string m_InjectedText;
		std::string m_TargetText;
		const char *m_pTargetPtr;
	};
}

#endif//__IALLOC_STRING_PLUGIN_H__