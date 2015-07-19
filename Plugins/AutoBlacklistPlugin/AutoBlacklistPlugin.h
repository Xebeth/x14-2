/**************************************************************************
	created		:	2011-06-04
	filename	: 	AutoBlacklistPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Tell detect plugin
**************************************************************************/
#ifndef __AUTO_BLACKLIST_PLUGIN_H__
#define __AUTO_BLACKLIST_PLUGIN_H__

namespace Windower
{
	class PluginPropertyPage;

	class AutoBlacklistPlugin : public IGameChatPlugin, public CommandHandler
	{
		typedef std::vector<std::string> BlockedMessages;

		//! IDs of the commands registered with the plugin
		enum CommandMap
		{
			CMD_ADD = 0,		//!< add or updates the score of a word
			CMD_REMOVE,			//!< remove a word from the list
			CMD_LIST,			//!< list of blacklisted words
			CMD_THRESHOLD,		//!< update the threshold score
			CMD_BLOCK_AT,		//!< update the offenses count limit
			CMD_HISTORY,		//!< history of the blacklisted messages
			CMD_SCORE_MSG,		//!< scores a message according to the current settings
			CMD_COUNT			//!< number of registered commands
		};
		//! sub-commands for history
		enum HistorySubCmd
		{
			HISTORY_MSG = 0,	 //!< list of blocked messages
			HISTORY_OFFENDERS,	 //!< list of offenders and their count
		};

		class Offender
		{
		public:
			Offender(const std::string &Name_in, int Strikes_in)
				: m_Strikes(Strikes_in), m_Name(Name_in), m_bBlacklisted(false) {}

			const std::string& GetName() const { return m_Name; }
			bool IsBlacklisted() const { return m_bBlacklisted; }
			int GetStrikes() const { return m_Strikes; }

			void UpdateStrikes(int Mod_in, int Limit_in)
			{ m_Strikes += Mod_in; m_bBlacklisted = (m_Strikes >= Limit_in); }
			void AddBlacklistedMessage(std::string &ScoredMessage_in)
			{ m_BlockedMessages.push_back(ScoredMessage_in); }
			const BlockedMessages& GetBlockedMessages() const
			{ return m_BlockedMessages; }			

		private:
			BlockedMessages m_BlockedMessages;
			bool m_bBlacklisted;
			std::string m_Name;
			int m_Strikes;
		};
		
		typedef std::map<std::string, Offender*> Offenders;

	public:
		explicit AutoBlacklistPlugin(PluginFramework::IPluginServices *pServices_in);
		~AutoBlacklistPlugin();

		static PluginFramework::IPlugin* Create(PluginFramework::IPluginServices *pServices_in);
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);
		static void Destroy(PluginFramework::IPlugin *pInstance_in);

		DWORD_PTR OnChatMessage(CHAT_MESSAGE_TYPE MessageType_in, const char* pSender_in, DWORD_PTR MsgSize_in, const char *pOriginalMsg_in,
								char **pModifiedMsg_in_out, DWORD_PTR ModifiedSize_in, DWORD &MessageFlags_out);

	protected:
		int ScoreMessage(CHAT_MESSAGE_TYPE MessageType_in, const char *pOriginalMsg_in, std::string &ScoredMessage_out);
		bool ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string &Feedback_out);		
		Offender* AddOffender(const std::string &Sender_in);
		PluginPropertyPage* GetPropertyPage();
		bool UnregisterCommands();
		void OnSettingsChanged();
		bool RegisterCommands();
		bool Unsubscribe();
		bool Subscribe();
		// commands
		bool AddWord(const std::string &Word_in, long Score_in, std::string & Feedback_out);
		bool DisplayHistory(const std::string& SubCmd_in, std::string &Feedback_out);
		bool ScoreMessage(const char *pOriginalMsg_in, std::string & Feedback_out);
		bool RemoveWord(const std::string &Word_in, std::string & Feedback_out);
		bool UpdateThreshold(long Threshold_in, std::string & Feedback_out);
		bool UpdateCount(long Count_in, std::string & Feedback_out);		
		bool ListWords(std::string &Feedback_out);
		
		long m_BlacklistCount, m_BlacklistThreshold;
		ScoredWords *m_pScoredWords;
		Offenders m_Offenders;
	};
}

#endif//__AUTO_BLACKLIST_PLUGIN_H__