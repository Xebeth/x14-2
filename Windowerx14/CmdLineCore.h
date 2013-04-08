/**************************************************************************
	created		:	2013-03-24
	filename	: 	CmdLineCore.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Command line module
**************************************************************************/
#ifndef __CMD_LINE_CORE_H__
#define __CMD_LINE_CORE_H__
												 //53558BAC2488010000568BF133DB57					2013-03-22
#define PROCESS_CMD_OPCODES_SIGNATURE			"@@53558B????????????568BF133DB57"
#define PROCESS_CMD_OPCODES_SIGNATURE_OFFSET	-42
#define PROCESS_CMD_OPCODES_HOOK_SIZE			 11
#define PROCESS_CMD_HOOK "OnProcessCmd"

namespace Windower
{
	typedef struct _StringNode StringNode;
	class WindowerEngine;

	// char __thiscall sub_900110(void *this, int a2, int a3) => search Component\Shell\ShellCommandModule.cpp, 
	// the function makes reference to the offset below that (contains ' '	aka SpaceStrA)
	typedef bool (WINAPI *fnProcessCmd)(LPVOID pThis_in_out, StringNode* pCmd_in_out, char **pRawCmd_in);

	class CommandDispatcher;
	class CommandParser;	

	class CmdLineCore : public WindowerCore, public ICommandHandler
	{
		//! IDs of the commands registered with the plugin
		enum CommandMap
		{
			CMD_HELP = 0,			//!< displays help for commands
			CMD_LOAD_PLUGIN,		//!< loads a plugin
			CMD_UNLOAD_PLUGIN,		//!< unloads a plugin
			CMD_LIST_PLUGINS,		//!< list all plugins
			CMD_COUNT				//!< number of registered commands
		};
	public:
		CmdLineCore(WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out);
		~CmdLineCore();

		static bool WINAPI ProcessCmdHook(LPVOID pThis_in_out, StringNode* pCmd_in_out, char **pRawCmd_in);

		bool ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string& Feedback_out);

		// ICommandHandler interface implementation
		bool IsCommandValid(const WindowerCommand *pCommand_in) const;
		bool UnregisterCommand(WindowerCommand *pCommand_in);
		bool RegisterCommand(WindowerCommand *pCommand_in);
		// ICoreModule interface implementation
		void RegisterHooks(HookEngineLib::IHookManager &HookManager_in);
		void OnHookInstall(HookEngineLib::IHookManager &HookManager_in);

	protected:
		static bool FilterCommands(const StringNode *pCmd_in, std::string &Feedback_out);

		bool ShowCommandHelp(const std::string& CommandName_in, std::string& HelpMsg_out);
		int ShowCommandHelp(WindowerCommand *pCommand_in);
		bool UnregisterCommands();
		bool RegisterCommands();

		//! the command dispatcher
		static CommandDispatcher *m_pCommandDispatcher;
		//! the command parser
		static CommandParser *m_pCommandParser;
		//! trampoline to the original ProcessCmd function
		static fnProcessCmd m_pProcessCmdTrampoline;
	};
}

#endif//__CMD_LINE_CORE_H__
