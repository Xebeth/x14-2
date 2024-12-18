/**************************************************************************
	created		:	2011-06-04
	filename	: 	ChatLogPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Chat log plugin
**************************************************************************/
#include "stdafx.h"
#include <SettingsManager.h>

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
		: TimestampPlugin(pServices_in), m_bOpened(false), m_pFile(nullptr), m_pTimestamp(nullptr)
	{
		// create the log directory
		m_LogPath = format(_T("%s%s"), IPlugin::GetWorkingDir(), _T("logs"));
		CreateDirectory(m_LogPath.c_str(), nullptr);
		// convert the timestamp
		convert_utf8(m_TimestampFormat, m_TimestampFormatW);
	}

	//! \brief ChatLogPlugin destructor
	ChatLogPlugin::~ChatLogPlugin()
	{
		StopLog();

		if (m_pTimestamp != nullptr)
		{
			delete[] m_pTimestamp;
			m_pTimestamp = nullptr;
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
		if (pInstance_in != nullptr)
		{
			delete pInstance_in;
			pInstance_in = nullptr;
		}
	}

	/*! \brief Fills a VersionInfo structure with the plugin information
		\param[out] Info_out : a VersionInfo structure
	*/
	void ChatLogPlugin::Query(PluginInfo& PluginInfo_out)
	{
		PluginInfo_out.SetCompatibilityFlags(PLUGIN_COMPATIBILITY_WINDOWER | PLUGIN_COMPATIBILITY_NO_WIZARD);
		PluginInfo_out.SetDesc(_T("This plugin will log the content of the game chat"));
		PluginInfo_out.SetIdentifier(_T("745E1230-0C81-4220-B099-3A3392EFA03A"));		
		PluginInfo_out.SetVersion(PLUGIN_VERSION);
		PluginInfo_out.SetAuthor(_T("Xebeth`"));
		PluginInfo_out.SetName(_T("ChatLog"));
	}
	
	/*! \brief Callback invoked when the game chat receives a new line
		\param[in] MessageType_in : the type of the message
		\param[in] pSender_in : the sender of the message
		\param[in] pOriginalMsg_in : a pointer to the unmodified message
		\param[in] pModifiedMsg_in_out : the resulting text modified by the plugin
		\param[in] dwNewSize_out : the new size of the message
		\param[in] DWORD ModifiedSize_in : the modified message size
		\return the new size of the message
	*/
	DWORD_PTR ChatLogPlugin::OnChatMessage(CHAT_MESSAGE_TYPE MessageType_in, const char* pSender_in, DWORD_PTR MsgSize_in, const char *pOriginalMsg_in,
										   char **pModifiedMsg_in_out, DWORD_PTR ModifiedSize_in, DWORD &MessageFlags_out)
	{
		if (StartLog() && pOriginalMsg_in != nullptr && MsgSize_in > 0U)
		{
			string_t Sender, Message, Line;

			// update the timestamp
			UpdateTimestamp();
			// convert to Unicode
			convert_utf8(pSender_in, Sender);
			convert_utf8(pOriginalMsg_in, Message);
			// format the line
			if (Sender.empty())
				format(m_Buffer, _T("%s%s\r\n"), m_pTimestamp, Message.c_str());
			else
				format(m_Buffer, _T("%s%s: %s\r\n"), m_pTimestamp, Sender.c_str(), Message.c_str());
			// remove any non-printable character
			purge<wchar_t>(m_Buffer, _ALPHA | _SPACE | _PUNCT | _DIGIT, _T("\r\n\t"), true);
			// write the line in the log
			WriteLine(m_Buffer);
		}

		return MsgSize_in;
	}

	/*! \brief Writes a line in the log file
		\param[in] Line_in : the line to write in the log file
		\return true if the line was written successfully; false otherwise
	*/
	bool ChatLogPlugin::WriteLine(const string_t &Line_in)
	{
		if (m_pFile != nullptr)
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
		if (m_bOpened == false && m_pFile == nullptr)
		{
			string_t LogFilename;
			SYSTEMTIME Time;

			GetSystemTime(&Time);
			// create the file in the logs directory
			format(LogFilename, _T("%s\\chatlog_%d-%02d-%02d.log"), m_LogPath.c_str(), 
				   Time.wYear, Time.wMonth, Time.wDay);
			// open the file in shared mode for reading only
			m_pFile = _wfsopen(LogFilename.c_str(), _T("a+, ccs=UTF-8"), _SH_DENYWR);
			m_bOpened = (m_pFile != nullptr);

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
		if (m_bOpened && m_pFile != nullptr)
		{
			WriteLine(_T("== End of session =======================\r\n"));

			fclose(m_pFile);
			m_pFile = nullptr;
		}
	}

	void ChatLogPlugin::UpdateTimestamp()
	{
		if (m_pTimestamp == nullptr)
		{
			m_pTimestamp = new TCHAR[m_TimestampLength + 1];
			// clear the buffer
			memset(m_pTimestamp, 0, (m_TimestampLength + 1) * sizeof(TCHAR));
		}

		// get the current time
		GetTimeFormatW(LOCALE_SYSTEM_DEFAULT, 0UL, nullptr,
					   m_TimestampFormatW.c_str(),
					   m_pTimestamp, (int)m_TimestampLength + 1);
	}
}

using Windower::ChatLogPlugin;

/*! \brief Function exposed by the plugin DLL to initialize the plugin object
	\param[out] RegisterParams_out : Registration structure to be able to use the plugin
	\return true if the initialization succeeded; false otherwise
*/
extern "C" PLUGIN_API bool InitPlugin(PluginFramework::RegisterParams &RegisterParams_out)
{
	return PluginFramework::IPlugin::Initialize(RegisterParams_out, ChatLogPlugin::Create,
												ChatLogPlugin::Destroy, ChatLogPlugin::Query,
												Windower::ConfigurablePlugin::Configure);
}