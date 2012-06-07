/**************************************************************************
	created		:	2011-11-27
	filename	: 	ICommandHandler.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Base interface for the command handler class
**************************************************************************/
#ifndef __ICOMMAND_HANDLER_H__
#define __ICOMMAND_HANDLER_H__

namespace Windower
{
	class WindowerCommand;
	class CommandParameter;

	//! a hash map of command parameters
	typedef stdext::hash_map<std::string, CommandParameter*> CommandParams;

	class ICommandHandler
	{
	public:
		virtual int DefaultHandler(WindowerCommand *pCommand_in, std::string &Feedback_out);

		/*! \brief Executes the command specified by its ID
			\param[in] CmdID_in : the ID of the command to execute
			\param[in] Parameters_in : the parameters of the command
			\param[out] Feedback_out : the result of the execution
			\return true if the command was executed successfully; false otherwise
		*/
		virtual bool ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string &Feedback_out) =0;
		/*! \brief Verifies that the specified command is valid and is compatible with the invoker
			\param[in] pCommand_in : the command to validate
			\return true if the command is valid; false otherwise
		*/
		virtual bool IsCommandValid(const WindowerCommand *pCommand_in) const =0;
		/*! \brief Unregister the specified command with the command dispatcher
			\param[in] pCommand_in : the command to unregister
			\return true if the command was unregistered successfully; false otherwise
		*/
		virtual bool UnregisterCommand(WindowerCommand *pCommand_in) =0;
		/*! \brief Register the specified command with the command dispatcher
			\param[in] pCommand_in : the command to register
			\return true if the command was registered successfully; false otherwise
		*/
		virtual bool RegisterCommand(WindowerCommand *pCommand_in) =0;
	};
}

#endif//__ICOMMAND_HANDLER_H__