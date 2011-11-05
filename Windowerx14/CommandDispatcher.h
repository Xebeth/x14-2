/**************************************************************************
	created		:	2011-06-06
	filename	: 	CommandDispatcher.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Command dispatcher
**************************************************************************/
#ifndef __COMMAND_DISPATCHER_H__
#define __COMMAND_DISPATCHER_H__

namespace Windower
{
	//! hash map of registered commands
	typedef stdext::hash_map<std::string, WindowerCommand*> RegisteredCommands;
	//! set of plugin keys
	typedef std::set<DWORD> AuthorizedKeys;

	//! \brief Command dispatcher
	class CommandDispatcher : public WindowerCore
	{
	public:
		explicit CommandDispatcher(PluginEngine &Engine_in_out);
		~CommandDispatcher();

		// ICoreModule interface implementation

		/*! \brief Register the hooks for this module
			\param[in] HookManager_in : the hook manager
		*/
		void RegisterHooks(IHookManager &HookManager_in) {}
		/*! \brief Callback invoked when the hooks of the module are installed
			\param[in] HookManager_in : the hook manager
		*/
		void OnHookInstall(IHookManager &HookManager_in) {}

		int Dispatch(const WindowerCommand &Command_in);
		bool UnregisterCommand(unsigned long RegistrationKey_in, const std::string &CommandName_in);
		bool RegisterCommand(unsigned long RegistrationKey_in, const std::string &Name_in, const std::string &Description_in,
							 CallerParam Caller_in, fnCommandCallback CallbackFunc_in, unsigned int MinParamsCount_in = 0,
							 unsigned int MaxParamsCount = 0, const CommandParameters &Parameters_in = CommandParameters(),
							 bool Public_in = true, bool Restricted_in = false);
		WindowerCommand* FindCommand(const std::string &Name_in);
		bool Invoke(const string_t &ServiceName_in, const PluginFramework::ServiceParam &Params_in, PluginFramework::ServiceParam &Results_out);

		bool ShowCommandHelp(const std::string &CommandName_in, std::string &HelpMsg_out);
		static int ShowCommandHelp(const WindowerCommand *pCommand_in);

	protected:
		/*! \brief Registers the specified command in the dispatcher
			\param[in] Command_in : the command to register
			\return true if the command was registered successfully; false otherwise
		*/
		inline bool RegisterCommand(const WindowerCommand &Command_in)
		{
			return RegisterCommand(Command_in.RegistrationKey, Command_in.Name, Command_in.Description, Command_in.Caller,
								   Command_in.CommandCallbackFunc, Command_in.MinParamsCount, Command_in.MaxParamsCount,
								   Command_in.Parameters, Command_in.Public, Command_in.Restricted);
		}

		void InsertCommand(WindowerCommand *pCommand_in);
		void RemoveCommand(WindowerCommand *pCommand_in);
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