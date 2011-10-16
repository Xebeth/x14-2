/**************************************************************************
	created		:	2011-06-03
	filename	: 	WindowerEngine.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower x14 engine
**************************************************************************/
#ifndef __WINDOWER_ENGINE_H__
#define __WINDOWER_ENGINE_H__

#define PLUGIN_REGKEY	0xDEADBEEF

namespace Windower
{
	class CommandDispatcher;
	class WindowerProfile;
	class WindowerCommand;
	class SettingsManager;
	class PluginServices;
	class CommandParser;
	class GameChatCore;
	class GraphicsCore;
	class ICoreModule;
	class SystemCore;

	typedef stdext::hash_map<string_t, PluginFramework::IPlugin*> WindowerPlugins;
	typedef stdext::hash_map<string_t, PluginInfo> RegisteredPlugins;

	class WindowerEngine : public PluginEngine
	{
	public:
		explicit WindowerEngine(const TCHAR *pConfigFile_in);
		virtual ~WindowerEngine();

		virtual bool Attach();
		virtual bool Detach();

		bool InitializeEngine();
		void ShutdownEngine();
		void UpdateEngine();
		DWORD MainThread();

		void OnShutdown();

		SystemCore&				System()   const { return *m_pSystemCore; }
		GraphicsCore&			Graphics() const { return *m_pGraphicsCore; }
		GameChatCore&			GameChat() const { return *m_pGameChatCore; }
		const WindowerProfile&	Settings() const { return *m_pSettings; }

		void AddFeedbackMessage(const std::string &Feedback_in) { m_FeedbackMessages.push_back(Feedback_in); }

		// Command callbacks
		static int UnloadPlugin(const WindowerCommand *pCommand_in);
		static int LoadPlugin(const WindowerCommand *pCommand_in);

		const char* OnCreateString(const char *pText_in, std::string &Text_out);

	private:
		SettingsManager		*m_pSettingsManager;
		WindowerProfile		*m_pSettings;
		HookEngine			*m_pHookManager;
		SystemCore			*m_pSystemCore;
		GraphicsCore		*m_pGraphicsCore;
		GameChatCore		*m_pGameChatCore;
		CommandDispatcher	*m_pCommandDispatcher;
		CommandParser		*m_pCommandParser;

		bool				 m_bInjectVersion;
		bool				 m_bThreadInit;
		bool				 m_bShutdown;
		HWND				 m_hGameWnd;
		DWORD				 m_dwPID;

		std::list<std::string> m_FeedbackMessages;
	};
}

#endif//__WINDOWER_ENGINE_H__