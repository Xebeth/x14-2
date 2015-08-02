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
	class WindowerCommand;
	class WindowerEngine;

	//! hash map of registered commands
	typedef hash_map<char, WindowerCommand*> RegisteredCommands;
	//! set of plugin keys
	typedef std::set<DWORD> AuthorizedKeys;

	//! \brief Command dispatcher
	class CommandDispatcher : public WindowerCore
	{
	public:
		CommandDispatcher();
		virtual ~CommandDispatcher();

		bool UnregisterCommand(DWORD RegistrationKey_in, const std::string& CommandName_in);
		bool IsCommandValid(const WindowerCommand *pCommand_in) const;
		bool UnregisterCommand(WindowerCommand *pCommand_in);
		bool RegisterCommand(WindowerCommand *pCommand_in);		

		WindowerCommand* FindCommand(const std::string& Name_in) const;

		bool Invoke(const string_t& ServiceName_in, PluginFramework::ServiceParam &Params_in);

		const RegisteredCommands& GetCommands() const { return m_Commands; }

		// ICoreModule implementation
		bool RegisterServices();

	protected:
		void InsertCommand(WindowerCommand *pCommand_in);
		
		/*! \brief Checks if the specified key is authorized with the command dispatcher
			\param[in] Key_in : the key to check
			\return true if the key is authorized; false otherwise
		*/
		bool IsKeyAuthorized(DWORD Key_in) { return (m_ValidKeys.find(Key_in) != m_ValidKeys.cend()); }

		//! hash map of registered commands
		RegisteredCommands m_Commands;
		//! set of authorized keys
		AuthorizedKeys	   m_ValidKeys;
	};
}

#endif//__COMMAND_DISPATCHER_H__