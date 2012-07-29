/**************************************************************************
	created		:	2011-06-04
	filename	: 	ChatLogPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Chat log plugin
**************************************************************************/
#include "stdafx.h"
#include <SettingsManager.h>
#include <PluginFramework.h>
#include <utf8_convert.h>

#include <IGameChatPlugin.h>
#include <CommandHandler.h>
#include <PluginEngine.h>

#include "TimestampSettings.h"
#include "TimestampPlugin.h"
#include "ChatLogPlugin.h"
#include "version.h"

using namespace PluginFramework;

namespace Windower
{
	/*! \brief ChatLogPlugin constructor
		\param[in] pServices_in : a pointer to the plugin services
	*/
	ChatLogPlugin::ChatLogPlugin(PluginFramework::IPluginServices *pServices_in)
		: TimestampPlugin(pServices_in), m_bOpened(false), m_pFile(NULL), m_pTimestamp(NULL)
	{
		m_TimestampFormatW = m_pSettings->GetFormat();
		CreateDirectory(_T("logs"), NULL);

		convert_utf8(TEXT_COLOR_START, m_ColorTagStart);
		convert_utf8(TEXT_COLOR_END, m_ColorTagEnd);
		convert_utf8(TEXT_CHAT_PAUSE, m_ChatPause);
	}

	//! \brief ChatLogPlugin destructor
	ChatLogPlugin::~ChatLogPlugin()
	{
		StopLog();

		if (m_pTimestamp != NULL)
		{
			delete[] m_pTimestamp;
			m_pTimestamp = NULL;
		}
	}

	/*! \brief Creates an instance of ChatLogPlugin
		\param[in] pServices_in : a pointer to the plugin services
		\return a pointer to the new ChatLogPlugin instance
	*/
	IPlugin* ChatLogPlugin::Create(PluginFramework::IPluginServices *pServices_in)
	{
		return new ChatLogPlugin(pServices_in);
	}

	/*! \brief Destroys an instance of ChatLogPlugin
		\param[in] pInstance_in : an instance of ChatLogPlugin
	*/
	void ChatLogPlugin::Destroy(IPlugin *pInstance_in)
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
	void ChatLogPlugin::Query(PluginInfo& PluginInfo_out)
	{
		PluginInfo_out.SetDesc(_T("This plugin will log the content of the game chat"));
		PluginInfo_out.SetIdentifier(_T("745E1230-0C81-4220-B099-3A3392EFA03A"));
		PluginInfo_out.SetVersion(PLUGIN_VERSION);
		PluginInfo_out.SetAuthor(_T("Xebeth`"));
		PluginInfo_out.SetName(_T("ChatLog"));
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
	DWORD ChatLogPlugin::OnChatMessage(USHORT MessageType_in, const StringNode *pSender_in,
									  const StringNode *pMessage_in_out, const char *pOriginalMsg_in,
									  DWORD dwOriginalMsgSize_in, char **pBuffer_in_out,
									  bool &Unsubscribe_out)
	{
		if (StartLog())
		{
			string_t Sender, Message, Line;
			size_t StrPos;

			UpdateTimestamp();
			convert_utf8(pSender_in->pResBuf, Sender);
			convert_utf8(pOriginalMsg_in, Message);

			// remove color tags
			StrPos = Message.find(m_ColorTagStart);

			if (StrPos != string_t::npos)
			{
				Message.replace(StrPos, TEXT_COLOR_START_LEN, _T(""));
				StrPos = Message.find(m_ColorTagEnd);

				if (StrPos != string_t::npos)
					Message.replace(StrPos, TEXT_COLOR_END_LEN, _T(""));
			}
			// remove chat log pause
			StrPos = Message.find(m_ChatPause);

			if (StrPos != string_t::npos)
				Message.replace(StrPos, TEXT_CHAT_PAUSE_LEN, _T(""));

			if (Sender.empty())
				format(m_Buffer, _T("%s%s\n"), m_pTimestamp, Message.c_str());
			else
				format(m_Buffer, _T("%s%s: %s\n"), m_pTimestamp, Sender.c_str(), Message.c_str());

			WriteLine(m_Buffer);
		}

		return dwOriginalMsgSize_in;
	}

	/*! \brief Writes a line in the log file
		\param[in] Line_in : the line to write in the log file
		\return true if the line was written successfully; false otherwise
	*/
	bool ChatLogPlugin::WriteLine(const string_t &Line_in)
	{
		if (m_pFile != NULL)
		{
			size_t DataWritten, BufferLength = Line_in.length();
			DataWritten = fwrite(Line_in.c_str(), sizeof(TCHAR), BufferLength - 1, m_pFile);

			fflush(m_pFile);

			return (DataWritten == BufferLength);
		}

		return false;
	}

	/*! \brief Starts a new logging session
		\return true if the log file is opened; false otherwise
	*/
	bool ChatLogPlugin::StartLog()
	{
		if (m_bOpened == false && m_pFile == NULL)
		{
			string_t LogFilename;
			SYSTEMTIME Time;

			GetSystemTime(&Time);
			format(LogFilename, _T("logs\\chatlog_%d-%02d-%02d.log"), Time.wYear, Time.wMonth, Time.wDay);
			// open the file in shared mode for reading only
			m_pFile = _wfsopen(LogFilename.c_str(), _T("a+, ccs=UTF-8"), _SH_DENYWR);
			m_bOpened = (m_pFile != NULL);

			if (m_bOpened)
			{
				UpdateTimestamp();

				WriteLine(format(m_Buffer, _T("== New session : %ld-%02d-%02d %s ==\r\n"),
								 Time.wYear, Time.wMonth, Time.wDay, m_pTimestamp));
			}
		}

		return m_bOpened;
	}

	//! \brief Stops the current logging session
	void ChatLogPlugin::StopLog()
	{
		if (m_bOpened && m_pFile != NULL)
		{
			WriteLine(_T("== End of session =======================\r\n"));

			fclose(m_pFile);
			m_pFile = NULL;
		}
	}

	void ChatLogPlugin::UpdateTimestamp()
	{
		if (m_pTimestamp == NULL)
			m_pTimestamp = new TCHAR[m_TimestampLength + 1];

		// clear the buffer
		memset(m_pTimestamp, 0, m_TimestampLength + 1);
		// get the current time
		GetTimeFormatW(LOCALE_INVARIANT, NULL, NULL,
					   m_TimestampFormatW.c_str(),
					   m_pTimestamp, m_TimestampLength);
	}
}

using Windower::TimestampPlugin;
using Windower::ChatLogPlugin;

/*! \brief Function exposed by the plugin DLL to initialize the plugin object
	\param[out] RegisterParams_out : Registration structure to be able to use the plugin
	\return true if the initialization succeeded; false otherwise
*/
extern "C" PLUGIN_API bool InitPlugin(PluginFramework::RegisterParams &RegisterParams_out)
{
	return PluginFramework::IPlugin::Initialize(RegisterParams_out, ChatLogPlugin::Create,
												ChatLogPlugin::Destroy, ChatLogPlugin::Query,
												TimestampPlugin::Configure);
}