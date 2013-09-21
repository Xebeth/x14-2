/**************************************************************************
	created		:	2011-11-05
	filename	: 	CommandHandler.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Base class for objects handling windower commands
**************************************************************************/
#ifndef __COMMAND_HANDLER_H__
#define __COMMAND_HANDLER_H__

#ifndef _WINDOWER_PLUGIN_SDK_INCLUDE_
	#error Please include the global header file 'PluginSDK.h'
#endif

namespace Windower
{
	class WindowerCommand;

	//! a map of registered commands (ID => command)
	typedef std::map<INT_PTR, WindowerCommand*> HandlerCommands;
	
	//! \brief Base class for objects handling windower commands
	class CommandHandler : public ICommandHandler
	{
	public:
		CommandHandler(DWORD RegistrationKey_in, const std::string &InvokerName_in);
		virtual ~CommandHandler();
		
	protected:
		WindowerCommand* RegisterCommand(INT_PTR CmdID_in, const std::string &CmdName_in, const std::string &CmdDesc_in);
		virtual bool IsCommandValid(const WindowerCommand *pCommand_in) const;
		const WindowerCommand* FindCommand(INT_PTR CommandID_in) const;
		virtual bool UnregisterCommand( WindowerCommand *pCommand_in);
		virtual bool RegisterCommand(WindowerCommand *pCommand_in);
		bool RevokeCommand(INT_PTR CommandID_in);
		virtual void Clear();

		//! the plugin registration key
		DWORD m_RegistrationKey;
		//! the internal name of the plugin
		std::string m_InvokerName;
		//! map of registered commands
		HandlerCommands m_Commands;

	private:
		//! CommandHandler default constructor
		CommandHandler();
	};
}

#endif//__COMMAND_HANDLER_H__