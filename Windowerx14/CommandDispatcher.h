/**************************************************************************
	created		:	2011-06-06
	filename	: 	CommandDispatcher.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Command dispatcher
**************************************************************************/
#ifndef __COMMAND_DISPATCHER_H__
#define __COMMAND_DISPATCHER_H__

#include "ICoreModule.h"
#include "WindowerCore.h"

#include "WindowerCommand.h"
#include "ICommandHandler.h"

namespace Windower
{
	class WindowerCommand;

	//! hash map of registered commands
	typedef stdext::hash_map<std::string, WindowerCommand*> RegisteredCommands;
	//! set of plugin keys
	typedef std::set<DWORD> AuthorizedKeys;

	//! \brief Command dispatcher
	class CommandDispatcher : public WindowerCore, public ICommandHandler
	{
		//! IDs of the commands registered with the plugin
		enum CommandMap
		{
			CMD_HELP = 0,	//!< displays help for commands
			CMD_COUNT		//!< number of registered commands
		};

	public:
		explicit CommandDispatcher(PluginEngine &Engine_in_out);

		// ICoreModule interface implementation

		/*! \brief Register the hooks for this module
			\param[in] HookManager_in : the hook manager
		*/
		void RegisterHooks(IHookManager &HookManager_in) {}
		/*! \brief Callback invoked when the hooks of the module are installed
			\param[in] HookManager_in : the hook manager
		*/
		void OnHookInstall(IHookManager &HookManager_in) {}

		bool UnregisterCommand(DWORD RegistrationKey_in, const std::string &CommandName_in);

		WindowerCommand* FindCommand(const std::string &Name_in) const;

		bool Invoke(const string_t &ServiceName_in, const PluginFramework::ServiceParam &Params_in);

		bool ShowCommandHelp(const std::string &CommandName_in, std::string &HelpMsg_out);
		int ShowCommandHelp(WindowerCommand *pCommand_in);

		// ICommandHandler interface implementation
		virtual bool ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string &Feedback_out);
		virtual bool IsCommandValid(const WindowerCommand *pCommand_in) const;
		virtual bool UnregisterCommand(WindowerCommand *pCommand_in);
		virtual bool RegisterCommand(WindowerCommand *pCommand_in);

	protected:
		void InsertCommand(WindowerCommand *pCommand_in);
		
		/*! \brief Checks if the specified key is authorized with the command dispatcher
			\param[in] Key_in : the key to check
			\return true if the key is authorized; false otherwise
		*/
		bool IsKeyAuthorized(DWORD Key_in) { return (m_ValidKeys.find(Key_in) != m_ValidKeys.end()); }

		//! hash map of registered commands
		RegisteredCommands m_Commands;
		//! set of authorized keys
		AuthorizedKeys	   m_ValidKeys;
	};
}

#endif//__COMMAND_DISPATCHER_H__