/**************************************************************************
	created		:	2011-06-04
	filename	: 	TimestampPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Timestamp plugin
**************************************************************************/
#include "stdafx.h"
#include "resource.h"

#include <PluginFramework.h>
#include <SettingsManager.h>

#include <WindowerCommand.h>
#include <CommandHandler.h>
#include <IGameChatPlugin.h>
#include <PluginEngine.h>

#include "TimestampPlugin.h"
#include "version.h"

#include "TimestampSettings.h"
#include "TimestampConfigDlg.h"

using namespace PluginFramework;

namespace Windower
{
	/*! \brief TimestampPlugin constructor
		\param[in] pServices_in : a pointer to the plugin services
	*/
	TimestampPlugin::TimestampPlugin(PluginFramework::IPluginServices *pServices_in) 
		: IGameChatPlugin(pServices_in), CommandHandler(0xAF8B3EE1, "TimestampPlugin"),
		  m_pSettings(new TimestampSettings(IPlugin::GetConfigFile(), NULL))
	{
		// retrieve the format from the settings
		if (m_pSettings != NULL)
			convert_ansi(m_pSettings->GetFormat(), m_TimestampFormat);
		else
			m_TimestampFormat = "[HH:mm:ss]";
		// add a space if there is none after the timestamp
		if (m_TimestampFormat.back() != ' ')
			m_TimestampFormat += ' ';
		
		m_TimestampLength = m_TimestampFormat.length();
	}

	//! \brief TimestampPlugin destructor
	TimestampPlugin::~TimestampPlugin()
	{
		if (m_pSettings != NULL)
		{
			delete m_pSettings;
			m_pSettings = NULL;
		}
	}


	/*! \brief Creates an instance of TimestampPlugin
		\param[in] pServices_in : a pointer to the plugin services
		\return a pointer to the new TimestampPlugin instance
	*/
	IPlugin* TimestampPlugin::Create(PluginFramework::IPluginServices *pServices_in)
	{
		return new TimestampPlugin(pServices_in);
	}

	/*! \brief Destroys an instance of TimestampPlugin
		\param[in] pInstance_in : an instance of TimestampPlugin
	*/
	void TimestampPlugin::Destroy(IPlugin *pInstance_in)
	{
		if (pInstance_in != NULL)
		{
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
		PluginInfo_out.SetVersion(PLUGIN_VERSION);
		PluginInfo_out.SetAuthor(_T("Xebeth`"));
		PluginInfo_out.SetName(_T("Timestamp"));
	}

	/*! \brief Opens the configuration screen of the plugin
		\param[in] pInstance_in : the instance of the plugin to configure
		\param[in] pUserData_in : a pointer to the user data to pass to the plugin
		\return true if the user validated the configuration screen; false otherwise
	*/
	bool TimestampPlugin::Configure(PluginFramework::IPlugin *pInstance_in, const LPVOID pUserData_in)
	{
		TimestampConfigDlg ConfigDlg(IPlugin::GetConfigFile(), reinterpret_cast<const TCHAR*>(pUserData_in));

		return (ConfigDlg.DoModal() == IDOK);
	}
	
	/*! \brief Registers the commands of the plugin with the command dispatcher
		\return true if all the commands were registered successfully; false otherwise
	*/
	bool TimestampPlugin::RegisterCommands()
	{
		// register the command
		WindowerCommand *pCommand = RegisterCommand(CMD_FORMAT, "timestamp::format", "sets the format of the timestamp");

		if (pCommand != NULL && pCommand->AddStringParam("format", false, m_TimestampFormat.c_str(),
														 "format of the timestamp\n"
														 "\t\t\xe2\x87\x92 see the remarks at http://tinyurl.com/gettimeformatex ") == false)
		{
			delete pCommand;
			pCommand = NULL;
		}

		return (pCommand != NULL);
	}

	/*! \brief Unregisters the commands of the plugin with the command dispatcher
		\return true if all the commands were unregistered successfully; false otherwise
	*/
	bool TimestampPlugin::UnregisterCommands()
	{
		return RevokeCommand(CMD_FORMAT);
	}

	/*! \brief Callback invoked when the game chat receives a new line
		\param[in] MessageType_in : the type of the message
		\param[in] pSender_in : the sender of the message
		\param[in,out] pMessage_in : the message
		\param[in] pOriginalMsg_in : a pointer to the unmodified message
		\param[in] dwOriginalMsgSize_in : the size of the original message
		\param[in] pBuffer_in_out : the resulting text modified by the plugin
		\param[in] Unsubscribe_out : flag specifying if the plugin wants to revoke its subscription to the hook
		\return the size of the message
	*/
	bool TimestampPlugin::OnChatMessage(USHORT MessageType_in, const StringNode* pSender_in,
										const StringNode* pMessage_in, const char *pOriginalMsg_in,
										DWORD dwOriginalMsgSize_in, char **pBuffer_in_out,
										DWORD &dwdwNewSize_out)
	{
		if (pMessage_in != NULL && pMessage_in->pResBuf != NULL && strlen(pMessage_in->pResBuf) > 0U)
		{
			// add 11 characters for the timestamp
			dwdwNewSize_out = pMessage_in->dwSize + m_TimestampLength;
			// allocate a new buffer
			char *pRealloc = (char*)realloc(*pBuffer_in_out, dwdwNewSize_out * sizeof(char));

			if  (pRealloc != NULL)
			{
				*pBuffer_in_out = pRealloc;
				// clear the buffer
				memset(*pBuffer_in_out, 0, dwdwNewSize_out);
				// get the current time
				GetTimeFormatA(LOCALE_INVARIANT, NULL, NULL,
							   m_TimestampFormat.c_str(),
							   *pBuffer_in_out, m_TimestampLength);
				// copy the original text
				memcpy_s(*pBuffer_in_out + m_TimestampLength,
						 pMessage_in->dwSize * sizeof(char),
						 pMessage_in->pResBuf, pMessage_in->dwSize);

				return true;
			}
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
				if (SetFormat(Command_in.GetStringValue("format")))
				{
					Feedback_out = "The format of timestamps has been set successfully.";

					return true;
				}
		}

		return false;
	}

	/*! \brief Sets the format of the timestamp
		\param[in] Format_in : the new format of the timestamp
		\return true if the timestamp format is valid; false otherwise
	*/
	bool TimestampPlugin::SetFormat(const std::string& Format_in)
	{
		bool Result = false;

		if (Format_in.empty() == false)
		{
			m_TimestampFormat = Format_in;

			if (m_pSettings != NULL)
			{
				string_t Format;

				convert_utf8(m_TimestampFormat, Format);
				m_pSettings->SetFormat(Format);

				Result = m_pSettings->Save();
			}
			// add a space if there is none after the timestamp
			if (m_TimestampFormat.back() != ' ')
				m_TimestampFormat += ' ';
			m_TimestampLength = m_TimestampFormat.length();
		}

		return Result;
	}
}

#ifndef NO_EXPORT
	using Windower::TimestampPlugin;

	/*! \brief Function exposed by the plugin DLL to initialize the plugin object
		\param[out] RegisterParams_out : Registration structure to be able to use the plugin
		\return true if the initialization succeeded; false otherwise
	*/
	extern "C" PLUGIN_API bool InitPlugin(PluginFramework::RegisterParams &RegisterParams_out)
	{
		return PluginFramework::IPlugin::Initialize(RegisterParams_out, TimestampPlugin::Create, TimestampPlugin::Destroy,
													TimestampPlugin::Query, TimestampPlugin::Configure);
	}
#endif // NO_EXPORT