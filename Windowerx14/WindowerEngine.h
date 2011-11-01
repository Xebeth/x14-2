/**************************************************************************
	created		:	2011-06-03
	filename	: 	WindowerEngine.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower x14 engine
**************************************************************************/
#ifndef __WINDOWER_ENGINE_H__
#define __WINDOWER_ENGINE_H__

//! Engine plugin key
#define PLUGIN_REGKEY	0xDEADBEEF

namespace Windower
{
	class CommandDispatcher;
	class WindowerCommand;
	class SettingsManager;
	class PluginServices;
	class InjectVersion;
	class CommandParser;
	class GameChatCore;
	class GraphicsCore;
	class ICoreModule;
	class SystemCore;
#ifdef _DEBUG
	class TestCore;
#endif // _DEBUG

	//! a map of plugins
	typedef stdext::hash_map<string_t, PluginFramework::IPlugin*> WindowerPlugins;
	//! a map of registered plugin info
	typedef stdext::hash_map<string_t, PluginInfo> RegisteredPlugins;

	//! \brief Windower x14 engine
	class WindowerEngine : public PluginEngine
	{
	public:
		explicit WindowerEngine(const TCHAR *pConfigFile_in);
		virtual ~WindowerEngine();

		virtual bool Attach();
		virtual bool Detach();

		void InitializeEngine();
		void ShutdownEngine();
		void UpdateEngine();
		DWORD MainThread();

		void OnShutdown();

#ifdef _DEBUG
		/*! \brief Retrieves the test core module
			\return the test core module
		*/
		TestCore& Test() const { return *m_pTestCore; }
#endif // _DEBUG
		/*! \brief Retrieves the system core module
			\return the system core module
		*/
		SystemCore& System() const { return *m_pSystemCore; }
		/*! \brief Retrieves the graphics core module
			\return the graphics core module
		*/
		GraphicsCore& Graphics() const { return *m_pGraphicsCore; }
		/*! \brief Retrieves the game chat core module
			\return the game chat core module
		*/
		GameChatCore& GameChat() const { return *m_pGameChatCore; }
		/*! \brief Retrieves the current settings
			\return the settings
		*/
		const WindowerProfile& Settings() const { return m_Settings; }
		/*! \brief Adds a message to the feedback queue
			\param[in] Feedback_in : the message to add to the queue
		*/
		void AddFeedbackMessage(const std::string &Feedback_in) { m_FeedbackMessages.push_back(Feedback_in); }

		// Command callbacks
		static int UnloadPlugin(const WindowerCommand *pCommand_in);
		static int LoadPlugin(const WindowerCommand *pCommand_in);

	private:
		//! internal plugin used to inject the windower version in the main menu screen
		InjectVersion *m_pInjectVersion;
		//! the settings manager
		SettingsManager *m_pSettingsManager;
		//! the current settings
		WindowerProfile m_Settings;
		//! the hook engine
		HookEngine m_HookManager;
		//! the test core module
#ifdef _DEBUG
		TestCore *m_pTestCore;
#endif // _DEBUG
		//! the system core module
		SystemCore *m_pSystemCore;
		//! the graphics core module
		GraphicsCore *m_pGraphicsCore;
		//! the game chat core module
		GameChatCore *m_pGameChatCore;
		//! the command dispatcher
		CommandDispatcher *m_pCommandDispatcher;
		//! the command parser
		CommandParser *m_pCommandParser;
		//! flag specifying if the engine thread has been initialized
		bool m_bThreadInit;
		//! flag specifying if the game is shutting down
		bool m_bShutdown;
		//! a handle to the game window
		HWND m_hGameWnd;
		//! the process ID of the game
		DWORD m_dwPID;
		//! the feedback queue
		std::list<std::string> m_FeedbackMessages;
	};
}

#endif//__WINDOWER_ENGINE_H__