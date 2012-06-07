/**************************************************************************
	created		:	2011-11-05
	filename	: 	CommandHandler.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Base class for objects handling windower commands
**************************************************************************/
#ifndef __WINDOWER_PLUGIN_H__
#define __WINDOWER_PLUGIN_H__

#include "ICommandHandler.h"

namespace Windower
{
	using namespace PluginFramework;

	class WindowerCommand;

	//! a map of registered commands (ID => command)
	typedef std::map<INT_PTR, WindowerCommand*> HandlerCommands;
	
	//! \brief Base class for objects handling windower commands
	class CommandHandler : public ICommandHandler
	{
	public:
		CommandHandler(DWORD RegistrationKey_in, const std::string &InvokerName_in);
		virtual ~CommandHandler();
		virtual void Clear();

		WindowerCommand* RegisterCommand(INT_PTR CmdID_in, const std::string &CmdName_in, const std::string &CmdDesc_in);
		bool RevokeCommand(INT_PTR CommandID_in);

		const WindowerCommand* FindCommand(INT_PTR CommandID_in) const;

	protected:
		virtual bool IsCommandValid(const WindowerCommand *pCommand_in) const;
		virtual bool UnregisterCommand( WindowerCommand *pCommand_in);
		virtual bool RegisterCommand(WindowerCommand *pCommand_in);

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

#endif//__WINDOWER_PLUGIN_H__