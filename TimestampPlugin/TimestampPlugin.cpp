/**************************************************************************
	created		:	2011-06-04
	filename	: 	TimestampPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Timestamp plugin
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <FormatChatMessageHook.h>
#include <IGameChatPlugin.h>
#include "WindowerCommand.h"
#include "TimestampPlugin.h"

const PluginFramework::IPluginServices* PluginFramework::IPlugin::m_pPluginServices = NULL;

namespace Windower
{
	// TimestampPlugin default constructor
	TimestampPlugin::TimestampPlugin() : IGameChatPlugin(),
		m_TimestampFormat(TIMESTAMP_DEFAULT_FORMAT),
		m_TimestampLength(TIMESTAMP_DEFAULT_LENGTH) {}

	/*! \brief Creates an instance of TimestampPlugin
		\return a pointer to the new TimestampPlugin instance
	*/
	PluginFramework::IPlugin* TimestampPlugin::Create()
	{
		TimestampPlugin *pNewInst = new TimestampPlugin;
		TimestampPlugin::Query(pNewInst->m_PluginInfo);

		if (m_pPluginServices->SubscribeService(_T("GameChat"), _T("OnChatMessage"), pNewInst) == false)
		{
			delete pNewInst;
			pNewInst = NULL;
		}

		CallerParam Caller("TimestampPlugin", pNewInst);
		const char *pParamName = "format";
		CommandParameters Params;

		Params[pParamName].Name = pParamName;
		Params[pParamName].Type = COMMAND_PARAM_TYPE_STRING;
		Params[pParamName].Value = TIMESTAMP_DEFAULT_FORMAT;
		Params[pParamName].Description = "format of the timestamp\n\t\t-- see the remarks at http://tinyurl.com/gettimeformatex ";
		WindowerCommand Command(PLUGIN_REGKEY, "timestamp::format", "sets the format of the timestamp", Caller, SetFormat, Params, 1, 1);
		PluginFramework::ServiceParam InvokeArg(_T("WindowerCommand"), &Command);

		if (m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("RegisterCommand"), InvokeArg, PluginFramework::ServiceParam()) == false)
		{
			delete pNewInst;
			pNewInst = NULL;
		}
			
		return pNewInst;
	}

	/*! \brief Destroys an instance of TimestampPlugin
		\param[in] pInstance_in : an instance of TimestampPlugin
	*/
	void TimestampPlugin::Destroy(PluginFramework::IPlugin *pInstance_in)
	{
		if (pInstance_in != NULL)
		{
			UnregisterParam UnregParam(PLUGIN_REGKEY, "timestamp::format");
			PluginFramework::ServiceParam InvokeArg(_T("UnregisterParam"), &UnregParam);

			m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("UnregisterCommand"), InvokeArg, PluginFramework::ServiceParam());
			m_pPluginServices->UnsubscribeService(_T("GameChat"), _T("OnChatMessage"), pInstance_in);

			delete pInstance_in;
			pInstance_in = NULL;
		}
	}

	/*! \brief Fills a PluginInfo structure with the plugin information
		\param[out] Info_out : a PluginInfo structure
	*/
	void TimestampPlugin::Query(PluginInfo& Info_out)
	{
		Info_out.Author = _T("Xebeth`");
		Info_out.Name = _T("Timestamp");
		Info_out.PluginVersion.FromString(_T("1.0.0"));
		Info_out.FrameworkVersion.FromString(_T("1.0.0"));
		Info_out.Descritpion = _T("This plugin will add a timestamp to the chat log");
		Info_out.PluginIdentifier.FromString(_T("AF8B3EE1-B092-45C7-80AA-A2BF2213DA2B"));
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

	/*! \brief SetFormat command invocation
		\param[in] pCommand_in : the command received from the command dispatcher
		\return DISPATCHER_RESULT_SUCCESS if successful; DISPATCHER_RESULT_INVALID_CALL otherwise
	*/
	int TimestampPlugin::SetFormat(const WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL && pCommand_in->Caller.DataType.compare("TimestampPlugin") == 0)
		{
			TimestampPlugin *pTimestamp = reinterpret_cast<TimestampPlugin*>(pCommand_in->Caller.pData);
			const WindowerCommandParam *pParam = pCommand_in->GetParameter("format");

			if (pTimestamp != NULL && pParam != NULL)
			{
				if (pTimestamp->SetFormat(pParam->Value))
					return DISPATCHER_RESULT_SUCCESS;
			}

			return DISPATCHER_RESULT_INVALID_PARAMETERS;
		}

		return DISPATCHER_RESULT_INVALID_CALL;
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

/*! \brief Function exposed by the plugin DLL to retrieve the plugin information
	\param[in] PluginInfo_out : the plugin information
*/
extern "C" PLUGIN_API void QueryPlugin(PluginInfo &Info_out)
{
	TimestampPlugin::Query(Info_out);
}

/*! \brief Function exposed by the plugin DLL to initialize the plugin object
	\param[in] PluginServices_in : the plugin services
	\return a pointer to the plugin registration parameters if successful; NULL otherwise
*/
extern "C" PLUGIN_API RegisterParams* InitPlugin(const PluginFramework::IPluginServices &ServicesParams_in)
{
	RegisterParams *pParams = new RegisterParams;

	TimestampPlugin::Query(pParams->Info);

	pParams->QueryFunc = TimestampPlugin::Query;
	pParams->CreateFunc = TimestampPlugin::Create;
	pParams->DestroyFunc = TimestampPlugin::Destroy;

	TimestampPlugin::SetPluginServices(ServicesParams_in);

	return pParams;
}