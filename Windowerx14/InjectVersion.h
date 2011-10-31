/**************************************************************************
	created		:	2011-10-18
	filename	: 	InjectVersion.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Engine internal plugin used to inject the windower version in the main menu screen
**************************************************************************/
#ifndef _INJECTOR_VERSION_H__
#define _INJECTOR_VERSION_H__

namespace Windower
{
	//! \brief Engine internal plugin used to inject the windower version in the main menu screen
	class InjectVersion : public ICreateTextNodePlugin
	{
	public:
		explicit InjectVersion(PluginFramework::IPluginServices *pPluginServices_in);

		const char* OnCreateTextNode(const char *pText_in, bool &Unsubscribe_out);

		/*! \brief Callback invoked when the game chat receives a new line
			\param[in] MessageType_in : the type of the message
			\param[in] pSender_in : the sender of the message
			\param[in,out] pMessage_in_out : the message (might have been modified by other plugins)
			\param[in] pOriginalMsg_in : a pointer to the unmodified message
			\param[in] dwOriginalMsgSize_in : the size of the original message
			\param[in] pBuffer_in_out : the resulting text modified by the plugin
			\param[in] Unsubscribe_out : 
			\return true if the message was logged; false otherwise
		*/
		virtual bool OnChatMessage(USHORT MessageType_in, const StringNode* pSender_in_out,
								   StringNode* pMessage_in_out, const char *pOriginalMsg_in,
								   DWORD dwOriginalMsgSize_in, char **pBuffer_in_out,
								   bool &Unsubscribe_out) { return true; }
	};
}

#endif//_INJECTOR_VERSION_H__