/**************************************************************************
	created		:	2011-06-03
	filename	: 	WindowerEngine.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower x14-2 engine
**************************************************************************/
#ifndef __WINDOWER_ENGINE_H__
#define __WINDOWER_ENGINE_H__

#define ENGINE_KEY 0xDEADBEEF

class Timer;

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
	class CmdLineCore;
	class ICoreModule;
	class SystemCore;
	class PlayerCore;
#ifdef _DEBUG
	class TestCore;
#endif // _DEBUG

	//! a map of plugins
	typedef stdext::hash_map<string_t, PluginFramework::IPlugin*> WindowerPlugins;
	typedef std::vector<DWORD_PTR> MemoryScanResult;

	//! \brief Windower x14-2 engine
	class WindowerEngine : public PluginEngine
	{
	public:
		WindowerEngine(HMODULE hModule_in, const TCHAR *pConfigFile_in);
		~WindowerEngine();

		// thread safety
		inline bool UnlockEngineThread();
		inline void LockEngineThread();

		void OnClose();
		DWORD MainThread();
		void ShutdownEngine();

		bool IsPlayerLoggedIn() const;

		// commands
		bool Exit(std::string& Feedback_out);

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
		/*! \brief Retrieves the player data core module
			\return the player data module
		*/
		PlayerCore& Player() const { return *m_pPlayerCore; }
		/*! \brief Retrieves the current settings
			\return the settings
		*/
		const WindowerProfile& Settings() const { return m_Settings; }

	private:
		bool Attach();
		bool Detach();

		bool InitializePlugins();
		void InitializeEngine();
		void UpdateEngine();

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
		//! the command line core module
		CmdLineCore *m_pCmdLineCore;
		//! the player data core module
		PlayerCore *m_pPlayerCore;
		//! a handle to the game window
		HWND m_hGameWnd;
		//! handle of the windower DLL
		HMODULE m_hWindowerDLL;
		//! the process ID of the game
		DWORD m_dwPID;
		//! update timer
		Timer *m_pUpdateTimer;
		//! critical section for plugin operations
		CRITICAL_SECTION m_PluginLock;
		//! flag controlling the lifetime of the engine thread
		volatile bool m_bShutdown;
	};
}

#endif//__WINDOWER_ENGINE_H__