/**************************************************************************
	created		:	2013-03-24
	filename	: 	CmdLineCore.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Command line module
**************************************************************************/
#ifndef __CMD_LINE_CORE_H__
#define __CMD_LINE_CORE_H__

#ifdef _M_X64
													 //48 8B 02 49 8B F0 48 8B FA 80 38 00			2015-07-16
	#define PROCESS_CMD_OPCODES_SIGNATURE			"##488B02498BF0488BFA803800"
	#define PROCESS_CMD_OPCODES_SIGNATURE_OFFSET	-23
	#define PROCESS_CMD_OPCODES_SIZE				 0 // 8
#else
													 //51 57 8B ?? 89 ?? ?? 89 ?? ?? 89				2015-07-16
	#define PROCESS_CMD_OPCODES_SIGNATURE			"##51578B??89????89????89"
	#define PROCESS_CMD_OPCODES_SIGNATURE_OFFSET	-39
	#define PROCESS_CMD_OPCODES_SIZE				 11
#endif // _M_X64

#define CMD_LINE_MODULE "CmdLine"
#define PROCESS_CMD_HOOK "OnProcessCmd"

#define CRAFTING_STATUS_MARKER		"status="
#define CRAFTING_STATUS_LENGTH		STRLEN(CRAFTING_STATUS_MARKER)

#define CRAFTING_CONDITION_MARKER	"cond="
#define CRAFTING_CONDITION_LENGTH	STRLEN(CRAFTING_CONDITION_MARKER)

#define CRAFTING_ABILITY_MARKER		"ja:"
#define CRAFTING_ABILITY_LENGTH		STRLEN(CRAFTING_ABILITY_MARKER)

namespace Windower
{
	typedef struct _StringNode StringNode;
	class WindowerEngine;

	// int __thiscall sub_B97B20(void *this, int *a2, int a3)
	// int __fastcall sub_1409672C0(__int64 a1, __int64 a2, __int64 a3)
	typedef int (WINAPI *fnProcessCmd)(LPVOID pThis_in_out, StringNode* pCmd_in_out, LPVOID pUnknown_in);

	class CommandDispatcher;
	class WindowerEngine;
	class CommandParser;

	class ConditionalAction
	{
	public:
		ConditionalAction(std::string Action_in, bool Overwrite_in, const std::list<std::string> &Values_in)
			: Action(Action_in), Overwrite(Overwrite_in), Values(Values_in) {}

		bool IsTrue(long Value_in) const
		{
			std::list<std::string>::const_iterator valueEndIt = Values.cend(), valueIt;

			for (valueIt = Values.cbegin(); valueIt != valueEndIt; ++valueIt)
			{
				if (Value_in == atol(valueIt->c_str()))
					return true;
			}

			return false;
		}

		std::list<std::string> Values;
		std::string Action;
		bool Overwrite;
	};

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
			CMD_MACRO,				//!< executes a macro
			CMD_MACRO_ABORT,		//!< aborts the current macro
			CMD_WAIT,				//!< wait command
			CMD_KEY_PRESS,			//!< key press command
			CMD_EXIT,				//!< exit the game
			CMD_WAIT_MSG,			//!< wait for a message
			CMD_MACRO_IF,			//!< condition for macros
			CMD_MACRO_ENDIF,		//!< end of condition for macros
			CMD_COUNT				//!< number of registered commands
		};

		CmdLineCore();
		~CmdLineCore();

		static int WINAPI ProcessCmdHook(LPVOID pThis_in_out, StringNode* pCmd_in_out, LPVOID pUnknown_in);
		static int InjectCommand(const std::string &Cmd_in);
		
		bool ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string& Feedback_out);
		bool ExecuteMacroFile(const string_t &macroFile_in, unsigned long repeat = 1UL);
		void SetMacroDirectry(const string_t &Directory_in);
		bool FindMacroFile(string_t &MacroFile_in_out);
		void AbortMacro();

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
		void MacroWait(long wait) const;
		bool UnregisterCommands();
		bool RegisterCommands();
		void ParseLine(const std::string &line);

		//! list of macro directories
		string_t m_MacroDirectory;
		//! conditional action
		ConditionalAction *m_pCondition;
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
