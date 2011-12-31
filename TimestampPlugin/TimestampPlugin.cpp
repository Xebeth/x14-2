/**************************************************************************
	created		:	2011-06-04
	filename	: 	TimestampPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Timestamp plugin
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>

#include <WindowerCommand.h>
#include <CommandHandler.h>
#include <IGameChatPlugin.h>
#include <PluginEngine.h>

#include "TimestampPlugin.h"

using namespace PluginFramework;

namespace Windower
{
	// TimestampPlugin default constructor
	TimestampPlugin::TimestampPlugin() : IGameChatPlugin(), CommandHandler(0xAF8B3EE1, "TimestampPlugin"),
		m_TimestampFormat(TIMESTAMP_DEFAULT_FORMAT),
		m_TimestampLength(TIMESTAMP_DEFAULT_LENGTH) {}

	/*! \brief Creates an instance of TimestampPlugin
		\return a pointer to the new TimestampPlugin instance
	*/
	IPlugin* TimestampPlugin::Create()
	{
		TimestampPlugin *pNewInst = new TimestampPlugin;
		WindowerCommand *pCommand;

		// initialize the plugin
		TimestampPlugin::Query(pNewInst->m_PluginInfo);
		// subscribe to the chat service
		pNewInst->Subscribe();
		// register the command
		pCommand = pNewInst->RegisterCommand(CMD_FORMAT, "timestamp::format", "sets the format of the timestamp");

		if (pCommand != NULL)
		{
			// create the parameters
			pCommand->AddStringParam("format", false, TIMESTAMP_DEFAULT_FORMAT,
									 "format of the timestamp\n"
									 "\t\t-- see the remarks at http://tinyurl.com/gettimeformatex ");
		}

		return pNewInst;
	}

	/*! \brief Destroys an instance of TimestampPlugin
		\param[in] pInstance_in : an instance of TimestampPlugin
	*/
	void TimestampPlugin::Destroy(IPlugin *pInstance_in)
	{
		if (pInstance_in != NULL)
		{
			TimestampPlugin *pTimestamp = static_cast<TimestampPlugin*>(pInstance_in);
			// revoke the command
			pTimestamp->RevokeCommand(CMD_FORMAT);
			// unsubscribe from the chat service
			pTimestamp->Unsubscribe();

			delete pInstance_in;
			pInstance_in = NULL;
		}
	}

	/*! \brief Fills a VersionInfo structure with the plugin information
		\param[out] Info_out : a VersionInfo structure
	*/
	void TimestampPlugin::Query(PluginInfo& PluginInfo_out)
	{
		PluginInfo_out.SetDesc(_T("This plugin will add a timestamp to the chat log"));
		PluginInfo_out.SetIdentifier(_T("AF8B3EE1-B092-45C7-80AA-A2BF2213DA2B"));
		PluginInfo_out.SetAuthor(_T("Xebeth`"));
		PluginInfo_out.SetName(_T("Timestamp"));
		PluginInfo_out.SetVersion(_T("1.0.0"));

		IPlugin::Query(PluginInfo_out);
	}

	/*! \brief Callback invoked when the game chat receives a new line
		\param[in] MessageType_in : the type of the message
		\param[in] pSender_in : the sender of the message
		\param[in,out] pMessage_in_out : the message (might have been modified by other plugins)
		\param[in] pOriginalMsg_in : a pointer to the unmodified message
		\param[in] dwOriginalMsgSize_in : the size of the original message
		\param[in] pBuffer_in_out : the resulting text modified by the plugin
		\param[in] Unsubscribe_out : flag specifying if the plugin wants to revoke its subscription to the hook
		\return true if the message was logged; false otherwise
	*/
	bool TimestampPlugin::OnChatMessage(USHORT MessageType_in, const StringNode* pSender_in_out,
										StringNode* pMessage_in_out, const char *pOriginalMsg_in,
										DWORD dwOriginalMsgSize_in, char **pBuffer_in_out,
										bool &Unsubscribe_out)
	{
		if (pMessage_in_out->pResBuf != NULL)
		{
			DWORD dwNewSize;
			// add 11 characters for the timestamp
			dwNewSize = pMessage_in_out->dwSize + m_TimestampLength;
			// allocate a new buffer
			*pBuffer_in_out = (char*)realloc(*pBuffer_in_out, dwNewSize * sizeof(char));
			// clear the buffer
			memset(*pBuffer_in_out, 0, dwNewSize);
			// get the current time
			GetTimeFormatA(LOCALE_INVARIANT, NULL, NULL, m_TimestampFormat.c_str(),
						   *pBuffer_in_out, m_TimestampLength + 1);
			// copy the original text
			memcpy_s(*pBuffer_in_out + m_TimestampLength,
					 pMessage_in_out->dwSize * sizeof(char),
					 pMessage_in_out->pResBuf, pMessage_in_out->dwSize);
			// replace the object data with timestamp + text
			pMessage_in_out->pResBuf = *pBuffer_in_out;
			pMessage_in_out->dwSize = dwNewSize;

			return true;
		}

		return false;
	}

	
	/*! \brief Executes the command specified by its ID
		\param[in] CmdID_in : the ID of the command to execute
		\param[in] Command_in : the command to execute
		\param[out] Feedback_out : the result of the execution
		\return true if the command was executed successfully; false otherwise
	*/
	bool TimestampPlugin::ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string &Feedback_out)
	{
		switch(CmdID_in)
		{
			case CMD_FORMAT:
				return SetFormat(Command_in.GetStringValue("format"));
		}

		return false;
	}

	/*! \brief Sets the format of the timestamp
		\param[in] Format_in : the new format of the timestamp
		\return true if the timestamp format is valid; false otherwise
	*/
	bool TimestampPlugin::SetFormat(const std::string& Format_in)
	{
		if (Format_in.empty() == false)
		{
			m_TimestampLength = Format_in.length();
			m_TimestampFormat = Format_in;

			return true;
		}

		return false;
	}
}

using Windower::TimestampPlugin;

/*! \brief Function exposed by the plugin DLL to initialize the plugin object
	\param[in] pServices_in : services used to (un)subscribe to services and invoke them
	\return a pointer to the plugin registration parameters if successful; NULL otherwise
*/
extern "C" PLUGIN_API RegisterParams* InitPlugin(IPluginServices *pServices_in)
{
	RegisterParams *pParams = new RegisterParams;

	TimestampPlugin::Query(pParams->Info);
	IPlugin::SetServices(pServices_in);

	pParams->QueryFunc = TimestampPlugin::Query;
	pParams->CreateFunc = TimestampPlugin::Create;
	pParams->DestroyFunc = TimestampPlugin::Destroy;

	return pParams;
}