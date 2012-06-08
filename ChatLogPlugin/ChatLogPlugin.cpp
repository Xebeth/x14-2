/**************************************************************************
	created		:	2011-06-04
	filename	: 	ChatLogPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Chat log plugin
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>

#include <IGameChatPlugin.h>
#include <PluginEngine.h>

#include "ChatLogPlugin.h"
#include "version.h"

using namespace PluginFramework;

namespace Windower
{
	/*! \brief ChatLogPlugin constructor
		\param[in] pServices_in : a pointer to the plugin services
	*/
	ChatLogPlugin::ChatLogPlugin(PluginFramework::IPluginServices *pServices_in)
		: IGameChatPlugin(pServices_in)
	{
		CreateDirectory(_T("logs"), NULL);
		m_bOpened = false;
		m_pFile = NULL;
	}

	//! \brief ChatLogPlugin destructor
	ChatLogPlugin::~ChatLogPlugin()
	{
		StopLog();
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
	bool ChatLogPlugin::OnChatMessage(USHORT MessageType_in, const StringNode *pSender_in,
									  StringNode *pMessage_in_out, const char *pOriginalMsg_in,
									  DWORD dwOriginalMsgSize_in, char **pBuffer_in_out,
									  bool &Unsubscribe_out)
	{
		if (StartLog())
			return WriteLine(format(m_Buffer, _T("%S: %S\r\n"), pSender_in->pResBuf, pOriginalMsg_in));

		return false;
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
			DataWritten = fwrite(Line_in.c_str(), BufferLength * sizeof(TCHAR), BufferLength, m_pFile);

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

			m_bOpened = (_wfopen_s(&m_pFile, LogFilename.c_str(), _T("a+, ccs=UTF-8")) == 0);

			if (m_bOpened)
			{
				WriteLine(format(m_Buffer, _T("== New session : %ld-%02d-%02d %02d:%02d:%02d ==\r\n"),
								 Time.wYear, Time.wMonth, Time.wDay, Time.wHour, Time.wMinute, Time.wSecond));
			}
		}

		return m_bOpened;
	}

	//! \brief Stops the current logging session
	void ChatLogPlugin::StopLog()
	{
		if (m_bOpened && m_pFile != NULL)
		{
			WriteLine(_T("== End of session =============================="));

			fclose(m_pFile);

			delete m_pFile;
			m_pFile = NULL;
		}
	}
}

using Windower::ChatLogPlugin;

/*! \brief Function exposed by the plugin DLL to initialize the plugin object
	\return a pointer to the plugin registration parameters if successful; NULL otherwise
*/
extern "C" PLUGIN_API RegisterParams* InitPlugin()
{
	return PluginFramework::IPlugin::Initialize(ChatLogPlugin::Create, ChatLogPlugin::Destroy,
												ChatLogPlugin::Query, NULL);
}