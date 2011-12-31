/**************************************************************************
	created		:	2011-06-04
	filename	: 	ICreateTextNodePlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Base class for plugins subscribing to the CreateTextNode service
**************************************************************************/
#ifndef __IALLOC_STRING_PLUGIN_H__
#define __IALLOC_STRING_PLUGIN_H__

#include "IGameChatPlugin.h"

namespace Windower
{
	//! \brief Base class for plugins subscribing to the CreateTextNode service
	class ICreateTextNodePlugin : public IGameChatPlugin
	{
	public:
		/*! \brief ICreateTextNodePlugin constructor
			\param[in] TargetText_in : the target text in which to inject data
		*/
		explicit ICreateTextNodePlugin(const std::string &TargetText_in)
			: m_TargetText(TargetText_in), m_pTargetPtr(NULL) {}
		//! \brief ICreateTextNodePlugin destructor
		virtual ~ICreateTextNodePlugin() { m_pTargetPtr = NULL; }

		/*! \brief Callback function invoked when the game creates a string object
			\param[in] pText_in : the text used to create the string node
			\param[in] Unsubscribe_out : flag specifying if the plugin wants to revoke its subscription to the hook
			\return the modified version of the text
		*/
		virtual const char* OnCreateTextNode(const char *pText_in, bool &Unsubscribe_out) =0;

	protected:
		virtual bool Unsubscribe();
		virtual bool Subscribe();

		//! buffer used to inject text in the text node
		std::string m_InjectedText;
		//! buffer containing the target text
		std::string m_TargetText;
		//! cached pointer for the target text (to avoid string comparisons)
		const char *m_pTargetPtr;
	};
}

#endif//__IALLOC_STRING_PLUGIN_H__