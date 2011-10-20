/**************************************************************************
	created		:	2011-10-18
	filename	: 	WindowerVersionInjector.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#ifndef __WINDOWER_VERSION_INJECTOR_H__
#define __WINDOWER_VERSION_INJECTOR_H__

namespace Windower
{
	class WindowerVersionInjector : public ICreateXmlNodePlugin
	{
	public:
		explicit WindowerVersionInjector(PluginFramework::IPluginServices *pPluginServices_in);

		const char* OnCreateXmlNode(const char *pText_in, bool &Unsubscribe_out);

		virtual bool FormatChatMessage(USHORT MessageType, const StringObject* pSender_in_out,
									   StringObject* pMessage_in_out, const char *pOriginalMsg_in,
									   DWORD dwOriginalMsgSize, char **pBuffer_in_out) { return true; }
	};
}

#endif//__WINDOWER_VERSION_INJECTOR_H__