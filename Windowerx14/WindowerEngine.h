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

		void ShutdownEngine(bool UnloadDLL_in = false);
		bool Attach();
		bool Detach();

		// thread safety
		inline bool UnlockEngineThread();
		inline void LockEngineThread();
		// main engine thread
		DWORD MainThread();

		bool IsPlayerLoggedIn() const;

		DWORD MemoryScan(const std::string &Pattern_in,
						 MemoryScanResult &Results_in_out);

		// commands
		bool Exit(std::string& Feedback_out);

	private:
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
		// flag specifying if the engine has been detached
		volatile bool m_bDetached;
	};
}

#endif//__WINDOWER_ENGINE_H__