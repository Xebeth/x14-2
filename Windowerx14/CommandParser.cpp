/**************************************************************************
	created		:	2011-06-06
	filename	: 	CommandParser.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Command Parser
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <HookEngine.h>

#include <CommandHandler.h>
#include "WindowerSettings.h"
#include "WindowerEngine.h"

#include "WindowerCommand.h"
#include "ICoreModule.h"
#include "WindowerCore.h"
#include "WindowerCommand.h"
#include "CommandDispatcher.h"
#include "CommandParser.h"

namespace Windower
{
	/*! \brief CommandParser constructor
		\param[in] Engine_in_out : the windower engine
		\param[in] Dispatcher_in : the command dispatcher
	*/
	CommandParser::CommandParser(WindowerEngine &Engine_in_out, CommandDispatcher &Dispatcher_in)
		: WindowerCore(Engine_in_out), m_CommandDispatcher(Dispatcher_in)
	{
		m_Engine.RegisterModule(_T("CommandParser"), this);
	}

	/*! \brief Parses a raw command (e.g. chat line) and returns the arguments and a feedback message
		\param[in] pRawCommand_in : the raw command buffer
		\param[out] Command_out : the resulting command after parsing
		\param[out] pFeedbackMsg_out : feedback message as a result of the command parsing
		\param[out] FeedbackMsgSize_out : the size of the feedback message
		\return the number of parameters found for the command or a value <0 otherwise if parsing failed
	*/
	int CommandParser::ParseCommand(const char *pRawCommand_in, WindowerCommand &Command_out,
									char **pFeedbackMsg_out, DWORD &FeedbackMsgSize_out)
	{
		std::string RawCommand, FeedbackMsg, CommandName;
		std::queue<std::string> Params;
		WindowerCommand *pCommand;
		int Result;

		// shouldn't happen but skip the // if they're still present
		if (strstr(pRawCommand_in, "//") == pRawCommand_in)
			RawCommand = pRawCommand_in + 2;
		else
			RawCommand = pRawCommand_in;

		Result = Tokenize(RawCommand, CommandName, Params);
		// set the name of the command
		Command_out.SetName(CommandName);

		if ((pCommand = m_CommandDispatcher.FindCommand(CommandName)) != NULL)
		{
			size_t ParamsCount = Params.size();

			if (pCommand != NULL)
				Command_out.Copy(*pCommand);
			else
				return PARSER_RESULT_INVALID_COMMAND;

			// validate the number of parameters for the command
			if (ParamsCount < Command_out.GetMinParams())
				Result = PARSER_RESULT_TOO_FEW_PARAMETERS;
			else if (ParamsCount > Command_out.GetMaxParams())
				Result = PARSER_RESULT_TOO_MANY_PARAMETERS;

			if (Result < 0)
			{
				return SetFeedback(Result, Command_out, ParamsCount, NULL, NULL,
								   pFeedbackMsg_out, FeedbackMsgSize_out);
			}
			else if (ParamsCount > 0)
			{
				const CommandParams &Parameters = Command_out.GetParameters();
				CommandParams::const_iterator ParamIter = Parameters.begin();
				char *pConvertTrail = NULL;
				std::string CurrentParam;
				int Error = 0;

				// validate each value and set it to the corresponding parameter
				do
				{
					CurrentParam = Params.front();
					Params.pop();

					switch(ParamIter->second->GetType())
					{
						case COMMAND_PARAM_TYPE_STRING:
						default:
							ParamIter->second->SetStringValue(CurrentParam);
						break;
						case COMMAND_PARAM_TYPE_INTEGER:
						{
							_set_errno(0);
							long Value = strtol(CurrentParam.c_str(), &pConvertTrail, 0);
							_get_errno(&Error);

							if (*pConvertTrail != NULL || Error != 0)
							{
								return SetFeedback(PARSER_RESULT_INVALID_INT_PARAMETER, Command_out, ParamsCount,
												   ParamIter->second->GetName().c_str(), CurrentParam.c_str(), 
												   pFeedbackMsg_out, FeedbackMsgSize_out);
							}

							ParamIter->second->SetIntegerValue(Value);
						}
						break;
						case COMMAND_PARAM_TYPE_FLOAT:
						{
							_set_errno(0);
							double Value = strtod(CurrentParam.c_str(), &pConvertTrail);
							_get_errno(&Error);

							if (*pConvertTrail != NULL || Error != 0)
							{
								return SetFeedback(PARSER_RESULT_INVALID_FLOAT_PARAMETER, Command_out, ParamsCount,
												   ParamIter->second->GetName().c_str(), CurrentParam.c_str(),
												   pFeedbackMsg_out, FeedbackMsgSize_out);
							}

							ParamIter->second->SetFloatValue(Value);
						}
						break;
					}

					++ParamIter;
				}
				while (Params.empty() == false && ParamIter != Parameters.end());
			}
		}

		return Result;
	}

	/*! \brief Sets the feedback message given the command and error code
		\param[in] ErrorCode_in : the error code from the parsing
		\param[in,out] Command_in_out : the command 
		\param[in] ParamCount_in : the number of parameters expected by the command
		\param[in] pParamName_in : the name of the parameter that provoked the error
		\param[in] pParamValue_in : the value of the parameter that provoked the error
		\param[out] pFeedbackMsg_out : the string receiving the error message
		\param[out] MsgSize_out : the size of the error message
		\return the error code
	*/
	int CommandParser::SetFeedback(int ErrorCode_in, WindowerCommand &Command_in_out, size_t ParamCount_in,
								   const char *pParamName_in, const char *pParamValue_in,
								   char **pFeedbackMsg_out, DWORD &MsgSize_out)
	{
		std::string Feedback;

		switch(ErrorCode_in)
		{
			case PARSER_RESULT_INVALID_FLOAT_PARAMETER:
				if (pParamName_in != NULL && pParamValue_in != NULL)
				{
					format(Feedback, "Parameter '%s' : invalid float value \u00AB%s\u00BB",
						   pParamName_in, pParamValue_in);
				}
				else
					Feedback = "Invalid float parameter";
			break;
			case PARSER_RESULT_INVALID_INT_PARAMETER:
				if (pParamName_in != NULL && pParamValue_in != NULL)
				{
					format(Feedback, "Parameter '%s' : invalid integer value \u00AB%s\u00BB",
						   pParamName_in, pParamValue_in);
				}
				else
					Feedback = "Invalid float parameter";
			break;
			case PARSER_RESULT_TOO_MANY_PARAMETERS:
				format(Feedback, "Too many parameters (%u) : expecting at most %u",
					   ParamCount_in, Command_in_out.GetMaxParams());
			break;
			case PARSER_RESULT_TOO_FEW_PARAMETERS:
				format(Feedback, "Too few parameters (%u) : expecting at least %u",
					   ParamCount_in, Command_in_out.GetMinParams());
			break;
		}

		if (Feedback.empty() == false)
		{
			*pFeedbackMsg_out = _strdup(Feedback.c_str());
			MsgSize_out = Feedback.size();
		}

		// invalidate the command
		Command_in_out.Invalidate();

		return ErrorCode_in;
	}

	/*! \brief Tokenizes a command and sets the resulting command and its parameters
		\param[in] pRawCommand_in : the raw command buffer
		\param[out] Command_out : the resulting command
		\param[out] Params_out : the parameters of the command
		\return the number of parameters
	*/
	int CommandParser::Tokenize(const std::string &RawCommand_in, std::string &Command_out, std::queue<std::string> &Params_out)
	{
		unsigned int ParamsCount = tokenize<char>(RawCommand_in, Params_out, " ", "\"");

		if (Params_out.empty() == false)
		{
			// the first token is the command
			Command_out = Params_out.front();
			// remove it from the queue
			Params_out.pop();
			--ParamsCount;
		}

		return ParamsCount;
	}
}