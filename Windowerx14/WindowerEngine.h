/**************************************************************************
	created		:	2011-06-03
	filename	: 	WindowerEngine.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower x14-2 engine
**************************************************************************/
#ifndef __WINDOWER_ENGINE_H__
#define __WINDOWER_ENGINE_H__

#include "WindowerSettings.h"
#include "PluginEngine.h"

#define ENGINE_KEY 0xDEADBEEF

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
#ifdef _DEBUG
	class TestCore;
#endif // _DEBUG

	//! a map of plugins
	typedef stdext::hash_map<string_t, PluginFramework::IPlugin*> WindowerPlugins;

	//! \brief Windower x14-2 engine
	class WindowerEngine : public PluginEngine
	{
	public:
		explicit WindowerEngine(HMODULE hModule_in, const TCHAR *pConfigFile_in);
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

		/*! \brief Optional callback to inform the engine that a successful call to the hook was made
			\param[in] pHookName_in : the name of the hook
		*/
		void OnHookCall(const char *pHookName_in);

	private:
		bool InitializePlugins();

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
		//! flag specifying if the engine thread has been initialized
		bool m_bThreadInit;
		//! flag specifying if the game is shutting down
		bool m_bShutdown;
		//! a handle to the game window
		HWND m_hGameWnd;
		//! the process ID of the game
		DWORD m_dwPID;
	};
}

#endif//__WINDOWER_ENGINE_H__