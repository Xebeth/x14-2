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
	typedef stdext::hash_map<std::string, WindowerCommand*> RegisteredCommands;
	typedef std::set<DWORD> AuthorizedKeys;
	//typedef std::queue<WindowerCommand*> CommandQueue;

	class CommandDispatcher : public WindowerCore
	{
		friend class CommandParser;
	public:
		CommandDispatcher(PluginEngine &Engine_in);
		~CommandDispatcher();

		// ICoreModule interface implementation
		void RegisterHooks(IHookManager *pHookManager){}
		void OnHookInstall(IHookManager *pHookManager){}

		int Dispatch(const WindowerCommand &Command_in);
		bool UnregisterCommand(unsigned long RegistrationKey_in, const std::string &CommandName_in);
		bool RegisterCommand(unsigned long RegistrationKey_in, const std::string &Name_in, const std::string &Description_in,
							 CallerParam Caller_in, fnCommandCallback CallbackFunc_in, unsigned int MinParamsCount_in = 0,
							 unsigned int MaxParamsCount = 0, const CommandParameters &Parameters_in = CommandParameters(),
							 bool Public_in = true, bool Restricted_in = false);
		WindowerCommand* FindCommand(const std::string &Name_in);
		bool Invoke(const string_t &ServiceName_in, const PluginFramework::ServiceParam &Params_in, PluginFramework::ServiceParam &Results_out);

	protected:
		const RegisteredCommands& GetRegisteredCommands() { return m_Commands; }

		inline bool RegisterCommand(const WindowerCommand &Command_in)
		{
			return RegisterCommand(Command_in.RegistrationKey, Command_in.Name, Command_in.Description, Command_in.Caller,
								   Command_in.CommandCallbackFunc, Command_in.MinParamsCount, Command_in.MaxParamsCount,
								   Command_in.Parameters, Command_in.Public, Command_in.Restricted);
		}

		void InsertCommand(WindowerCommand *pCommand_in);
		void RemoveCommand(WindowerCommand *pCommand_in);
		bool IsKeyAuthorized(DWORD Key_in) { return (m_ValidKeys.find(Key_in) != m_ValidKeys.end()); }

		RegisteredCommands m_Commands;
		AuthorizedKeys	   m_ValidKeys;
	};
}

#endif//__COMMAND_DISPATCHER_H__