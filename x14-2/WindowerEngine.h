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

	enum ThreadState
	{
		NONE = 0L,
		RUNNING,
		SUSPENDED,
		ABORTED,
		COUNT
	};

	//! a map of plugins
	typedef stdext::hash_map<string_t, PluginFramework::IPlugin*> WindowerPlugins;
	typedef std::pair<string_t, unsigned long> MacroParam;
	typedef std::vector<DWORD_PTR> MemoryScanResult;	

	//! \brief Windower x14-2 engine
	class WindowerEngine : public PluginEngine
	{
	public:
		WindowerEngine(const string_t &WorkingDir_in, const TCHAR *pConfigFile_in);
		~WindowerEngine();

		bool PressKey(long key, long delay, long repeat);
		void ShutdownEngine(bool UnloadDLL_in = false);
		void SetWnd(HWND hWnd_in);
		bool Attach();
		bool Detach();
		void Inject();

		// thread safety
		inline bool UnlockEngineThread();
		inline void LockEngineThread();
		// main engine thread
		DWORD MainThread();

		bool IsExiting() const { return m_bShutdown; }
		bool IsMacroThreadSuspended() const;
		bool IsMacroThreadActive() const;
		bool IsPlayerLoggedIn() const;
		bool AbortMacro();

		DWORD_PTR MemoryScan(const std::string &Pattern_in,
							 MemoryScanResult &Results_in_out);

		void UpdateMacroProgress(unsigned long step, unsigned long total, bool stop);
		bool CreateMacroThread(const string_t &file, unsigned long repeat);
		bool SuspendMacroThread(const std::string &condition);

		
		DWORD_PTR OnChatMessage(CHAT_MESSAGE_TYPE MessageType_in, const char* pSender_in, DWORD_PTR MsgSize_in, const char *pOriginalMsg_in,
								char **pModifiedMsg_in_out, DWORD_PTR ModifiedSize_in, DWORD &MessageFlags_out);

		// commands
		bool Exit(std::string& Feedback_out);

		bool DeserializeLabel(const string_t &Name_in, long &X_out, long &Y_out,
							  unsigned long &TextColor_out, string_t &FontName_out,
							  unsigned short &FontSize_out, bool &Bold_out,
							  bool &Italic_out, bool &Collapsed_out);
		void SerializeLabel(const string_t &Name_in, long X_in, long Y_in,
							unsigned long TextColor_in = 0xFF000000UL,
							const string_t &FontName_in = _T("Arial"),
							unsigned short FontSize_in = 12,
							bool Bold_in = true, bool Italic_in = false,
							bool Collapsed_in = false);

	private:
		bool InitializePlugins();
		void InitializeEngine();
		void UpdateEngine();

		template <typename T> class CallingContext
		{
		public:
			CallingContext() : m_pThis(NULL) {}

			bool IsSet() const { return (m_pThis != NULL); }
			void Set(T *pThis_in) { m_pThis = pThis_in; }
			T* operator->() const { return m_pThis; }
			operator T*() { return m_pThis; }

		private:
			T *m_pThis;
		};

		static DWORD WINAPI MacroThread(LPVOID pParam_in_out);
		bool SetMacroThreadState(ThreadState state);

		//! calling context
		static CallingContext<WindowerEngine> m_Context;		
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
		//! the process ID of the game
		DWORD m_dwPID;
		//! update timer
		Timer *m_pUpdateTimer;
		//! critical section for plugin operations
		CRITICAL_SECTION m_PluginLock;
		CRITICAL_SECTION m_MacroLock;
		volatile DWORD m_MacroThreadID;
		//! flag controlling the lifetime of the engine thread
		volatile bool m_bShutdown;
		// flag specifying if the engine has been detached
		volatile bool m_bDetached;
		volatile LONG m_MacroThreadState;
		UIAL::CUiWindow<> *m_pTextLabel;
		std::string m_ExpectCondition;
	};
}

#endif//__WINDOWER_ENGINE_H__