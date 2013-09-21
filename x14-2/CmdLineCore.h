/**************************************************************************
	created		:	2013-03-24
	filename	: 	CmdLineCore.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Command line module
**************************************************************************/
#ifndef __CMD_LINE_CORE_H__
#define __CMD_LINE_CORE_H__
												 //53558BAC2488010000568BF133DB57		-42 11		2013-03-22
												 //81ECD000000056578BBC24DC000000		-15 8		2013-03-22
												 //33C58945FC538B5D0C56578B7D088B07					2013-09-04
												 //8038008BF10F84400100008D4DDC33C05157	
#define PROCESS_CMD_OPCODES_SIGNATURE			"##33C58945??538B5D??56578B7D??8B07803800"
#define PROCESS_CMD_OPCODES_SIGNATURE_OFFSET	 -14
#define PROCESS_CMD_OPCODES_HOOK_SIZE			 9
#define PROCESS_CMD_HOOK "OnProcessCmd"
//																			   T e x t C o m m a n d
#define TEXT_COMMAND_SIGNATURE					"##400000000C00000000000000000154657874436F6D6D616E64"
#define TEXT_COMMAND_OFFSET						-0x164

#define CMD_LINE_MODULE "CmdLine"

namespace Windower
{
	typedef struct _StringNode StringNode;
	class WindowerEngine;

	// char __thiscall sub_900110(void *this, int a2, int a3) => search Component\Shell\ShellCommandModule.cpp, 
	// the function makes reference to the offset below that (contains ' '	aka SpaceStrA)
	typedef int (WINAPI *fnProcessCmd)(LPVOID pThis_in_out, StringNode* pCmd_in_out, LPVOID pUnknown_in);

	class CommandDispatcher;
	class WindowerEngine;
	class CommandParser;	

	class CmdLineCore : public WindowerCore, public ICommandHandler
	{
	public:
		//! IDs of the commands registered with the plugin
		enum CommandMap
		{
			CMD_HELP = 0,			//!< displays help for commands
			CMD_LOAD_PLUGIN,		//!< loads a plugin
			CMD_UNLOAD_PLUGIN,		//!< unloads a plugin
			CMD_LIST_PLUGINS,		//!< list all plugins
			CMD_CONFIGURE,			//!< configure a plugin
			CMD_EXIT,				//!< exit the game
			CMD_COUNT				//!< number of registered commands
		};

		CmdLineCore();
		~CmdLineCore();

		static int WINAPI ProcessCmdHook(LPVOID pThis_in_out, StringNode* pCmd_in_out, LPVOID pUnknown_in);
		static int InjectCommand(const std::string &Cmd_in);

		bool ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string& Feedback_out);

		// ICommandHandler interface implementation
		bool IsCommandValid(const WindowerCommand *pCommand_in) const;
		bool UnregisterCommand(WindowerCommand *pCommand_in);
		bool RegisterCommand(WindowerCommand *pCommand_in);
		// ICoreModule interface implementation
		void RegisterHooks(HookEngineLib::IHookManager &HookManager_in);
		void OnHookInstall(HookEngineLib::IHookManager &HookManager_in);

	protected:
		bool FilterCommands(const StringNode *pCmd_in, std::string &Feedback_out);

		bool ShowCommandHelp(const std::string& CommandName_in, std::string& HelpMsg_out);
		int ShowCommandHelp(WindowerCommand *pCommand_in);
		bool UnregisterCommands();
		bool RegisterCommands();

		//! the command dispatcher
		CommandDispatcher *m_pCommandDispatcher;
		//! the command parser
		CommandParser *m_pCommandParser;
		//! trampoline to the original ProcessCmd function
		fnProcessCmd m_pProcessCmdTrampoline;
		//! the object required for the command processing hook
		LPVOID m_pTextCmd, m_pTextCmdUnknown;
		//! calling context for the service hooks
		static CallingContext<CmdLineCore> m_Context;
	};
}

#endif//__CMD_LINE_CORE_H__
