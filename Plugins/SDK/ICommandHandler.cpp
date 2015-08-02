/**************************************************************************
	created		:	2011-11-27
	filename	: 	ICommandHandler.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <hash_map.h>

#include "ICommandHandler.h"
#include "CommandParameter.h"
#include "WindowerCommand.h"

namespace Windower
{
	/*! \brief Plugin commands handler
		\param[in] pCommand_in : the command received from the command dispatcher
		\param[out] Feedback_out : the result message of the execution
		\return DISPATCHER_RESULT_SUCCESS if successful; an error code otherwise
	*/
	int ICommandHandler::DefaultHandler(WindowerCommand *pCommand_in, std::string &Feedback_out)
	{
		// check that the command is correct within the context
		if (IsCommandValid(pCommand_in))
		{
			if (ExecuteCommand(pCommand_in->GetID(), *pCommand_in, Feedback_out))
				return DISPATCHER_RESULT_SUCCESS;
			else
				return DISPATCHER_RESULT_FAILURE;
		}
		else if (pCommand_in != NULL)
			format(Feedback_out, "The command '%s' is invalid.", pCommand_in->GetName().c_str());
		else
			Feedback_out = "Invalid command.";

		return DISPATCHER_RESULT_INVALID_CALL;
	}
}