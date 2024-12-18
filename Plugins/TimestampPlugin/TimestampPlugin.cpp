/**************************************************************************
	created		:	2011-06-04
	filename	: 	TimestampPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Timestamp plugin
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include "version.h"
#include <afxdlgs.h>

#include <PluginPropertyPage.h>

#include "TimestampPlugin.h"
#include "TimestampConfigDlg.h"

namespace Windower
{
	/*! \brief TimestampPlugin constructor
		\param[in] pServices_in : a pointer to the plugin services
	*/
	TimestampPlugin::TimestampPlugin(PluginFramework::IPluginServices *pServices_in) 
		: IGameChatPlugin(pServices_in), CommandHandler(0xAF8B3EE1, "TimestampPlugin"),
		  m_TimestampLength(0UL)

	{
		// create the settings
		if (pServices_in != nullptr)
		{
			// set the sound file path from the settings
			OnSettingsChanged();
		}
	}

	/*! \brief Creates an instance of TimestampPlugin
		\param[in] pServices_in : a pointer to the plugin services
		\return a pointer to the new TimestampPlugin instance
	*/
	PluginFramework::IPlugin* TimestampPlugin::Create(PluginFramework::IPluginServices *pServices_in)
	{
		return new TimestampPlugin(pServices_in);
	}

	/*! \brief Destroys an instance of TimestampPlugin
		\param[in] pInstance_in : an instance of TimestampPlugin
	*/
	void TimestampPlugin::Destroy(IPlugin *pInstance_in)
	{
		if (pInstance_in != nullptr)
		{
			delete pInstance_in;
			pInstance_in = nullptr;
		}
	}

	/*! \brief Fills a VersionInfo structure with the plugin information
		\param[out] Info_out : a VersionInfo structure
	*/
	void TimestampPlugin::Query(PluginFramework::PluginInfo& PluginInfo_out)
	{
		PluginInfo_out.SetDesc(_T("This plugin will add a timestamp to the chat log"));
		PluginInfo_out.SetIdentifier(_T("AF8B3EE1-B092-45C7-80AA-A2BF2213DA2B"));
		PluginInfo_out.SetCompatibilityFlags(PLUGIN_COMPATIBILITY_WINDOWER);
		PluginInfo_out.SetVersion(PLUGIN_VERSION);
		PluginInfo_out.SetAuthor(_T("Xebeth`"));
		PluginInfo_out.SetName(_T("Timestamp"));
	}

	/*! \brief Retrieves the property page used to configure the plugin
		\return a pointer to the property page of the plugin
	*/
	PluginPropertyPage* TimestampPlugin::GetPropertyPage()
	{
		return new TimestampConfigDlg(m_pSettings, m_PluginInfo.GetName());
	}

	//! \brief Callback function invoked when the settings have changed
	void TimestampPlugin::OnSettingsChanged()
	{
		// retrieve the format from the settings
		if (m_pSettings != nullptr)
			convert_ansi(m_pSettings->GetTimestampFormat(), m_TimestampFormat);
		else
			m_TimestampFormat = "[HH:mm:ss] ";
		// add a space if there is none after the timestamp
		if (m_TimestampFormat.back() != ' ')
			m_TimestampFormat += ' ';

		m_TimestampLength = m_TimestampFormat.length();
	}
	
	/*! \brief Registers the commands of the plugin with the command dispatcher
		\return true if all the commands were registered successfully; false otherwise
	*/
	bool TimestampPlugin::RegisterCommands()
	{
		// register the command
		WindowerCommand *pCommand = RegisterCommand(CMD_FORMAT, "timestamp::format", "sets the format of the timestamp");

		if (pCommand != nullptr && pCommand->AddStringParam("format", false, m_TimestampFormat.c_str(),
														 "format of the timestamp\n"
														 "      \xee\x81\xaf see the remarks at http://tinyurl.com/gettimeformatex ") == false)
		{
			delete pCommand;
			pCommand = nullptr;
		}

		return (pCommand != nullptr);
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
		\param[in] MsgSize_in : the size of the unmodified message
		\param[in] pOriginalMsg_in : a pointer to the unmodified message
		\param[in] pModifiedMsg_in_out : the resulting text modified by the plugin
		\param[in] dwNewSize_out : the new size of the message
		\param[in] DWORD ModifiedSize_in : the modified message size
		\return the new size of the message
	*/
	DWORD_PTR TimestampPlugin::OnChatMessage(CHAT_MESSAGE_TYPE MessageType_in, const char* pSender_in, DWORD_PTR MsgSize_in, const char *pOriginalMsg_in,
											 char **pModifiedMsg_in_out, DWORD_PTR ModifiedSize_in, DWORD &MessageFlags_out)
	{
		if (pOriginalMsg_in != nullptr && MsgSize_in > 1U)
		{
			// add 11 characters for the timestamp
			DWORD_PTR dwNewSize = ModifiedSize_in + m_TimestampLength;
			// allocate a new buffer
			if (ResizeBuffer(pOriginalMsg_in, MsgSize_in, dwNewSize, pModifiedMsg_in_out, ModifiedSize_in, m_TimestampLength))
			{
				// get the current time
				GetTimeFormatA(LOCALE_INVARIANT, NULL, nullptr,
							   m_TimestampFormat.c_str(),
							   *pModifiedMsg_in_out, (int)m_TimestampLength);

				return dwNewSize;
			}
		}

		return MsgSize_in;
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
			// add a space if there is none after the timestamp
			if (m_TimestampFormat.back() != ' ')
				m_TimestampFormat += ' ';
			m_TimestampLength = m_TimestampFormat.length();

			if (m_pSettings != nullptr)
			{
				string_t Format;

				convert_utf8(m_TimestampFormat, Format);
				m_pSettings->SetTimestampFormat(Format);

				Result = IPlugin::SaveSettings(m_pSettings);
			}
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
													TimestampPlugin::Query, Windower::ConfigurablePlugin::Configure);
	}
#endif // NO_EXPORT