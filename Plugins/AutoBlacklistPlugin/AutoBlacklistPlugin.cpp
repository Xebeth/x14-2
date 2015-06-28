/**************************************************************************
	created		:	2011-06-04
	filename	: 	AutoBlacklistPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Tell detect plugin
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <afxdlgs.h>

#include <PluginPropertyPage.h>

#include "AutoBlacklistConfigDlg.h"
#include "AutoBlacklistPlugin.h"
#include "version.h"
#include "Realloc.h"

#define CMD_LINE_MODULE		"CmdLine"
// available services for the module
#define PROCESS_CMD_HOOK	"OnProcessCmd"

namespace Windower
{
	//! \brief AutoBlacklistPlugin constructor
	AutoBlacklistPlugin::AutoBlacklistPlugin(PluginFramework::IPluginServices *pServices_in)
		: IGameChatPlugin(pServices_in), m_pScoredWords(NULL),
		  CommandHandler(0xBACA701F, "AutoBlacklistPlugin"),
		  m_BlacklistThreshold(15L), m_BlacklistCount(3L)
	{
		// create the settings
		if (pServices_in != NULL)
		{
			// set the sound file path from the settings
			OnSettingsChanged();
		}
	}
	
	AutoBlacklistPlugin::~AutoBlacklistPlugin()
	{
		Offenders::iterator OffenderIt, OffEndIt = m_Offenders.end();

		for (OffenderIt = m_Offenders.begin(); OffenderIt != OffEndIt; ++OffenderIt)
		{
			Offender *pOffender = OffenderIt->second;

			if (pOffender != NULL)
				delete pOffender;
		}
	}

	/*! \brief Adds the plugin as a subscriber to the game chat service
		\return true if the subscription succeeded; false otherwise
	*/
	bool AutoBlacklistPlugin::Subscribe()
	{
		return (__super::Subscribe() && SubscribeService(_T(CMD_LINE_MODULE), _T(PROCESS_CMD_HOOK)));
	}

	/*! \brief Removes the plugin as a subscriber to the game chat service
		\return true if the subscription was revoked successfully; false otherwise
	*/
	bool AutoBlacklistPlugin::Unsubscribe()
	{
		return (__super::Unsubscribe() && UnsubscribeService(_T(CMD_LINE_MODULE), _T(PROCESS_CMD_HOOK)));
	}

	/*! \brief Creates an instance of AutoBlacklistPlugin
		\param[in] pServices_in : a pointer to the plugin services
		\return a pointer to the new AutoBlacklistPlugin instance
	*/
	PluginFramework::IPlugin* AutoBlacklistPlugin::Create(PluginFramework::IPluginServices *pServices_in)
	{
		return new AutoBlacklistPlugin(pServices_in);
	}

	/*! \brief Destroys an instance of AutoBlacklistPlugin
		\param[in] pInstance_in : an instance of AutoBlacklistPlugin
	*/
	void AutoBlacklistPlugin::Destroy(IPlugin *pInstance_in)
	{
		if (pInstance_in != NULL)
		{
			delete pInstance_in;
			pInstance_in = NULL;
		}
	}

	/*! \brief Fills a VersionInfo structure with the plugin information
		\param[out] PluginInfo_out : a VersionInfo structure
	*/
	void AutoBlacklistPlugin::Query(PluginFramework::PluginInfo& PluginInfo_out)
	{
		PluginInfo_out.SetDesc(_T("This plugin will blacklist incoming chat messages based on score"));
		PluginInfo_out.SetIdentifier(_T("80C85F2E-BC84-4D29-9BB6-FFE43892CC50"));
		PluginInfo_out.SetCompatibilityFlags(PLUGIN_COMPATIBILITY_WINDOWER);
		PluginInfo_out.SetVersion(PLUGIN_VERSION);
		PluginInfo_out.SetName(_T("AutoBlacklist"));
		PluginInfo_out.SetAuthor(_T("Xebeth`"));
	}

	/*! \brief Registers the commands of the plugin with the command dispatcher
		\return true if all the commands were registered successfully; false otherwise
	*/
	bool AutoBlacklistPlugin::RegisterCommands()
	{
		bool Result = true;

		// add/update scored word
		WindowerCommand *pCommand = RegisterCommand(CMD_ADD, "blacklist::add", "adds a blacklisted word to the list or updates the score of an existing one");

		if (pCommand != NULL && (pCommand->AddStringParam("word", false, "", "the word to add to the list") == false
							  || pCommand->AddIntegerParam("score", false, 1L, "the score associated with the word") == false))
		{
			delete pCommand;
			pCommand = NULL;
			Result = false;
		}
		// remove word
		pCommand = RegisterCommand(CMD_REMOVE, "blacklist::remove", "removes a blacklisted word from the list");

		if (pCommand != NULL && pCommand->AddStringParam("word", false, "", "the word to remove from the list") == false)
		{
			delete pCommand;
			pCommand = NULL;
			Result = false;
		}
		// threshold score update
		pCommand = RegisterCommand(CMD_THRESHOLD, "blacklist::threshold", "updates the score threshold used to blacklist a message");

		if (pCommand != NULL && pCommand->AddIntegerParam("threshold", true, -1L, "the score threshold") == false)
		{
			delete pCommand;
			pCommand = NULL;
			Result = false;
		}
		// offense count update
		pCommand = RegisterCommand(CMD_BLOCK_AT, "blacklist::count", "updates the offense count used to add a character to the blacklist");

		if (pCommand != NULL && pCommand->AddIntegerParam("count", true, -1L, "the number of offenses before blacklisting a character (set to 0 to disable this behaviour)") == false)
		{
			delete pCommand;
			pCommand = NULL;
			Result = false;
		}
		// history
		pCommand = RegisterCommand(CMD_HISTORY, "blacklist::history", "gives a history of blocked messages and offenders");

		if (pCommand != NULL && pCommand->AddStringParam("sub-command", true, "msg", "valid sub-commands: msg (blacklisted messages), char (blacklisted characters)") == false)
		{
			delete pCommand;
			pCommand = NULL;
			Result = false;
		}
		// score message
		pCommand = RegisterCommand(CMD_SCORE_MSG, "blacklist::score", "evaluates the score of the provided message");

		if (pCommand != NULL && pCommand->AddStringParam("message", false, "", "the message to be scored") == false)
		{
			delete pCommand;
			pCommand = NULL;
			Result = false;
		}
		// list
		Result &= (RegisterCommand(CMD_LIST, "blacklist::list", "lists the blacklisted words and their scores") != NULL);

		return Result;
	}

	/*! \brief Executes the command specified by its ID
		\param[in] CmdID_in : the ID of the command to execute
		\param[in] Command_in : the command to execute
		\param[out] Feedback_out : the result of the execution
		\return true if the command was executed successfully; false otherwise
	*/
	bool AutoBlacklistPlugin::ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string &Feedback_out)
	{
		switch(CmdID_in)
		{
			case CMD_ADD:
				return AddWord(Command_in.GetStringValue("word"), Command_in.GetIntegerValue("score"), Feedback_out);
			case CMD_REMOVE:
				return RemoveWord(Command_in.GetStringValue("word"), Feedback_out);
			case CMD_LIST:
				return ListWords(Feedback_out);
			case CMD_THRESHOLD:
				return UpdateThreshold(Command_in.GetIntegerValue("threshold"), Feedback_out);
			case CMD_BLOCK_AT:
				return UpdateCount(Command_in.GetIntegerValue("count"), Feedback_out);
			case CMD_SCORE_MSG:
				return ScoreMessage(Command_in.GetStringValue("message").c_str(), Feedback_out);
			case CMD_HISTORY:
				return DisplayHistory(Command_in.GetStringValue("sub-command"), Feedback_out);
		}

		return false;
	}

	bool AutoBlacklistPlugin::UpdateThreshold(long Threshold_in, std::string & Feedback_out)
	{
		if (m_pSettings != NULL)
		{
			if (Threshold_in == -1L)
			{
				format(Feedback_out, "Score threshold is %ld.", m_BlacklistThreshold);
			}
			else
			{
				m_pSettings->SetBlacklistThreshold(Threshold_in);

				if (IPlugin::SaveSettings(m_pSettings))
				{
					format(Feedback_out, "Score threshold set to %ld.", Threshold_in);
					OnSettingsChanged();

					return true;
				}				
			}
		}

		return false;
	}

	bool AutoBlacklistPlugin::UpdateCount(long Count_in, std::string & Feedback_out)
	{
		if (m_pSettings != NULL)
		{
			if (Count_in == -1L)
			{
				format(Feedback_out, "Offense count is %ld.", m_BlacklistCount);
			}
			else
			{
				m_pSettings->SetBlacklistCount(Count_in);

				if (IPlugin::SaveSettings(m_pSettings))
				{
					format(Feedback_out, "Offense count set to %ld.", Count_in);
					OnSettingsChanged();

					return true;
				}
			}
		}

		return false;
	}

	bool AutoBlacklistPlugin::DisplayHistory(const std::string& SubCmd_in, std::string &Feedback_out)
	{
		if (SubCmd_in.compare("msg") == 0)
		{
			if (m_Offenders.empty())
			{
				Feedback_out = "No blacklisted message found.\n";
			}
			else
			{
				Feedback_out = "List of blacklisted messages by characters:\n";

				Offenders::const_iterator OffenderIt, OffEndIt = m_Offenders.cend();
				BlockedMessages::const_iterator MsgIt, EndMsgIt;

				for (OffenderIt = m_Offenders.cbegin(); OffenderIt != OffEndIt; ++OffenderIt)
				{
					Offender *pOffender = OffenderIt->second;

					if (pOffender != NULL)
					{
						const BlockedMessages &Messages = pOffender->GetBlockedMessages();

						if (Messages.empty() == false)
						{
							append_format(Feedback_out, " \xee\x81\xaf %s:", pOffender->GetName().c_str());
							EndMsgIt = Messages.cend();

							for (MsgIt = Messages.cbegin(); MsgIt != EndMsgIt; ++MsgIt)
								append_format(Feedback_out, "\n   %s\n", MsgIt->c_str());
						}
					}
				}
			}

			return true;
		}
		else if (SubCmd_in.compare("char") == 0)
		{
			if (m_Offenders.empty())
			{
				Feedback_out = "No blacklisted character found.\n";
			}
			else
			{
				Feedback_out = "List of offenders:\n";

				Offenders::const_iterator OffenderIt, OffEndIt = m_Offenders.cend();

				for (OffenderIt = m_Offenders.cbegin(); OffenderIt != OffEndIt; ++OffenderIt)
				{
					Offender *pOffender = OffenderIt->second;

					if (pOffender != NULL)
					{
						append_format(Feedback_out, " - %s (%ld/%ld): %sblacklisted\n",
									  pOffender->GetName().c_str(),
									  pOffender->GetStrikes(),
									  m_BlacklistCount, 
									  pOffender->IsBlacklisted() ? "" : "not ");
					}
				}
			}

			return true;
		}

		return false;
	}

	bool AutoBlacklistPlugin::ListWords(std::string &Feedback_out)
	{
		if (m_pScoredWords != NULL)
		{
			if (m_pScoredWords->empty())
			{
				Feedback_out = "The list of scored words is empty.";
			}
			else
			{
				ScoredWords::const_iterator WordIt, EndIt = m_pScoredWords->cend();
				std::string Line;

				Feedback_out =   "List of scored words used for blacklisting messages:";

				for(WordIt = m_pScoredWords->cbegin(); WordIt != EndIt; ++WordIt)
					append_format(Feedback_out, "\n   - '%s' (%ld)", WordIt->first.c_str(), WordIt->second);
			}

			return true;
		}

		return false;
	}

	bool AutoBlacklistPlugin::ScoreMessage(const char *pOriginalMsg_in, std::string &Feedback_out)
	{
		if (pOriginalMsg_in != NULL)
		{
			Feedback_out.clear();
			ScoreMessage(CHAT_MESSAGE_TYPE_INCOMING_TELL_MESSAGE, pOriginalMsg_in, Feedback_out);

			return true;
		}

		return false;
	}

	bool AutoBlacklistPlugin::RemoveWord(const std::string &Word_in, std::string & Feedback_out)
	{
		if (m_pScoredWords != NULL && m_pSettings != NULL)
		{
			ScoredWords::iterator WordIt = m_pScoredWords->find(Word_in);

			if (WordIt != m_pScoredWords->end())
			{
				std::string Word = WordIt->first;
				m_pScoredWords->erase(WordIt);

				if (IPlugin::SaveSettings(m_pSettings))
				{
					format(Feedback_out, "The word '%s' has been removed from the list.", Word.c_str());

					return true;
				}
			}
			else
			{
				format(Feedback_out, "The word '%s' doesn't exist in the list.", Word_in.c_str());

				return true;
			}
		}

		return false;
	}

	bool AutoBlacklistPlugin::AddWord(const std::string &Word_in, long Score_in, std::string & Feedback_out)
	{
		if (m_pScoredWords != NULL && m_pSettings != NULL)
		{
			ScoredWords::iterator WordIt;
			std::string Word = Word_in;

			std::transform(Word.begin(), Word.end(), Word.begin(), ::tolower);
			WordIt = m_pScoredWords->find(Word);

			if (Score_in <= 1L)
				Score_in = 1L;

			if (WordIt != m_pScoredWords->end())
				WordIt->second = Score_in;
			else
				(*m_pScoredWords)[Word] = Score_in;

			if(IPlugin::SaveSettings(m_pSettings))
			{
				format(Feedback_out, "The word '%s' has been added to the list with a score of %ld.", Word.c_str(), Score_in);

				return true;
			}
		}

		return false;
	}

	/*! \brief Unregisters the commands of the plugin with the command dispatcher
		\return true if all the commands were unregistered successfully; false otherwise
	*/
	bool AutoBlacklistPlugin::UnregisterCommands()
	{
		bool Result = true;

		// revoke the commands
		Result &= RevokeCommand(CMD_ADD);
		Result &= RevokeCommand(CMD_REMOVE);
		Result &= RevokeCommand(CMD_LIST);
		Result &= RevokeCommand(CMD_THRESHOLD);
		Result &= RevokeCommand(CMD_COUNT);
		Result &= RevokeCommand(CMD_SCORE_MSG);
		Result &= RevokeCommand(CMD_HISTORY);

		return Result;
	}
	
	/*! \brief Retrieves the property page used to configure the plugin
		\return a pointer to the property page of the plugin
	*/
	Windower::PluginPropertyPage* AutoBlacklistPlugin::GetPropertyPage()
	{
		if (m_pSettings != NULL)
			return new AutoBlacklistConfigDlg(m_pSettings, m_PluginInfo.GetName());

		return NULL;
	}

	//! \brief Callback function invoked when the settings have changed
	void AutoBlacklistPlugin::OnSettingsChanged()
	{
		if (m_pSettings != NULL)
		{
			m_BlacklistThreshold = m_pSettings->GetBlacklistThreshold();
			m_BlacklistCount = m_pSettings->GetBlacklistCount();
			m_pScoredWords = m_pSettings->GetScoredWords();

			if (m_BlacklistThreshold < 5L)
				m_BlacklistThreshold = 5L;
			if (m_BlacklistCount < 0L)
				m_BlacklistCount = 0L;
		}
	}

	/*! \brief Callback invoked when the game chat receives a new line
		\param[in] MessageType_in : the type of the message
		\param[in] pSender_in : the sender of the message
		\param[in] MsgSize_in : the size of the unmodified message
		\param[in] pOriginalMsg_in : a pointer to the unmodified message
		\param[in] pModifiedMsg_in_out : the resulting text modified by the plugin
		\param[in] DWORD ModifiedSize_in : the modified message size
		\return the new size of the message
	*/
	DWORD_PTR AutoBlacklistPlugin::OnChatMessage(USHORT MessageType_in, const char* pSender_in, DWORD_PTR MsgSize_in, const char *pOriginalMsg_in,
												 char **pModifiedMsg_in_out, DWORD_PTR ModifiedSize_in, DWORD &MessageFlags_out)
	{
		if (MessageType_in != CHAT_MESSAGE_TYPE_ECHO_MESSAGE
		 && MessageType_in != CHAT_MESSAGE_TYPE_SYSTEM_MESSAGE)
		{
			std::string ScoredMessage;
			int Score = ScoreMessage(MessageType_in, pOriginalMsg_in, ScoredMessage);

			if (Score >= m_BlacklistThreshold)
			{
				MessageType_in = CHAT_MESSAGE_TYPE_ECHO_MESSAGE;

				if (pSender_in != NULL && pModifiedMsg_in_out != NULL)
				{
					Offender *pOffender = AddOffender(pSender_in);

					if (pOffender != NULL)
					{
						pOffender->AddBlacklistedMessage(ScoredMessage);
						
						if (pOffender->IsBlacklisted() == false)
						{
							std::string NewMessage;
							size_t NewSize;

							pOffender->UpdateStrikes(1, m_BlacklistCount);

							format(NewMessage, "AutoBlacklist>> Message from %s blacklisted (score %i).\0", pOffender->GetName().c_str(), Score);
							MessageFlags_out |= MSG_FLAG_FORCE_ECHO;
							NewSize = NewMessage.size();

							if (ResizeBuffer(pOriginalMsg_in, MsgSize_in, NewSize, pModifiedMsg_in_out, ModifiedSize_in))
							{
								*pModifiedMsg_in_out = _strdup(NewMessage.c_str());
								MsgSize_in = NewSize;
							}
						}
						else
							MessageFlags_out |= MSG_FLAG_DISCARD;
					}
				}
			}
		}

		return MsgSize_in;
	}

	AutoBlacklistPlugin::Offender* AutoBlacklistPlugin::AddOffender(const std::string &Sender_in)
	{
		Offenders::iterator OffenderIt = m_Offenders.find(Sender_in);
		Offender *pOffender = NULL;

		if (OffenderIt != m_Offenders.end())
			pOffender = OffenderIt->second;
		else
			m_Offenders[Sender_in] = pOffender = new Offender(Sender_in, 0);

		return pOffender;
	}

	int AutoBlacklistPlugin::ScoreMessage(USHORT MessageType_in, const char *pOriginalMsg_in, std::string &ScoredMessage_out)
	{
		int Score = 0;

		if (m_pScoredWords != NULL && pOriginalMsg_in != NULL)
		{
			ScoredWords::const_iterator WordIt, WordEndIt = m_pScoredWords->cend();
			std::string Message = pOriginalMsg_in;
			std::map<size_t, long> Markers;
			size_t position, offset = 0U;
		
			switch(MessageType_in)
			{
				case CHAT_MESSAGE_TYPE_INCOMING_TELL_MESSAGE:
					ScoredMessage_out = "tell (+1) \xee\x81\xaf";
					offset += ScoredMessage_out.size();
					Score += 1;
				break;
				case CHAT_MESSAGE_TYPE_SHOUT_MESSAGE:
					ScoredMessage_out = "shout (+3) \xee\x81\xaf ";
					offset += ScoredMessage_out.size();
					Score += 3;
				break;
				case CHAT_MESSAGE_TYPE_YELL_MESSAGE:
					ScoredMessage_out = "yell (+5) \xee\x81\xaf ";
					offset += ScoredMessage_out.size();
					Score += 5;
				break;
			}
		
			for (WordIt = m_pScoredWords->cbegin(); WordIt != WordEndIt; ++WordIt)
			{
				position = Message.find(WordIt->first);

				while (position != std::string::npos)
				{
					Markers[position + WordIt->first.size()] = WordIt->second;
					position = Message.find(WordIt->first, position);
					Score += WordIt->second;
				}
			}

			std::map<size_t, long>::const_iterator MarkerIt, EndIt = Markers.cend();
			std::string ScoreHint;

			ScoredMessage_out += pOriginalMsg_in;

			for (MarkerIt = Markers.cbegin(); MarkerIt != EndIt; ++MarkerIt)
			{
				format(ScoreHint, "(+%ld) ", MarkerIt->second);
				ScoredMessage_out.insert(offset + MarkerIt->first, ScoreHint);
				offset += ScoreHint.size();
			}

			append_format(ScoredMessage_out, " \xee\x81\xaf total score: %ld", Score);
		}

		return Score;
	}
}

using Windower::AutoBlacklistPlugin;

/*! \brief Function exposed by the plugin DLL to initialize the plugin object
	\param[out] RegisterParams_out : Registration structure to be able to use the plugin
	\return true if the initialization succeeded; false otherwise
*/
extern "C" PLUGIN_API bool InitPlugin(PluginFramework::RegisterParams &RegisterParams_out)
{
	return PluginFramework::IPlugin::Initialize(RegisterParams_out, AutoBlacklistPlugin::Create, AutoBlacklistPlugin::Destroy,
												AutoBlacklistPlugin::Query, Windower::ConfigurablePlugin::Configure);
}