/**************************************************************************
	created		:	2011-06-06
	filename	: 	CommandParser.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Command Parser
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <NonCopyable.h>
#include <HookEngine.h>
#include <queue>

#include "BaseEngine.h"
#include "PluginEngine.h"
#include "WindowerEngine.h"

#include "WindowerCommand.h"
#include "ICoreModule.h"
#include "WindowerCore.h"
#include "WindowerCommand.h"
#include "CommandDispatcher.h"
#include "CommandParser.h"

namespace Windower
{
	CommandParser::CommandParser(WindowerEngine &Engine_in, CommandDispatcher &Dispatcher_in)
		: WindowerCore(Engine_in), m_CommandDispatcher(Dispatcher_in) {}

	/*! \brief Parses a raw command (e.g. chat line) and returns the arguments and a feedback message
		\return the number of parameters found for the command or a value <0 otherwise if parsing failed
	*/
	int CommandParser::ParseCommand(const char *pRawCommand_in, WindowerCommand &Command_out,
									char **pFeedbackMsg_out, DWORD &FeedbackMsgSize_out)
	{
		std::string RawCommand, FeedbackMsg, CommandName;
		RegisteredCommands::const_iterator Iter;
		std::queue<std::string> Params;
		WindowerCommand *pCommand;
		int Result;

		// shouldn't happen but skip the // if they're still present
		if (strstr(pRawCommand_in, "//") == pRawCommand_in)
			RawCommand = pRawCommand_in + 2;
		else
			RawCommand = pRawCommand_in;

		Result = Tokenize(RawCommand, CommandName, Params);

		if ((pCommand = m_CommandDispatcher.FindCommand(CommandName)) != NULL)
		{
			size_t ParamsCount = Params.size();

			if (pCommand != NULL)
				Command_out = *pCommand;
			else if (ParamsCount == 0 && CommandName.compare("help") == 0)
				Result = PARSER_RESULT_IMPLICIT_HELP;
			else
				return PARSER_RESULT_INVALID_COMMAND;

			// check if the implicit help command was invoked
			if (ParamsCount == 1 && Params.front().compare("help") == 0)
				Result = PARSER_RESULT_IMPLICIT_HELP;
			// validate the number of parameters for the command
			else if (ParamsCount < Command_out.MinParamsCount)
				Result = PARSER_RESULT_TOO_FEW_PARAMETERS;
			else if (ParamsCount > Command_out.MaxParamsCount)
				Result = PARSER_RESULT_TOO_MANY_PARAMETERS;

			if (Result < 0)
			{
				return SetFeedback(Result, Command_out, ParamsCount, NULL,
								   pFeedbackMsg_out, FeedbackMsgSize_out);
			}
			else if (ParamsCount > 0)
			{
				CommandParameters::iterator ParamIter = Command_out.Parameters.begin();
				char *pConvertTrail = NULL;
				std::string CurrentParam;
				int Error = 0;

				// validate each value and set it to the corresponding parameter
				do
				{
					CurrentParam = Params.front();
					Params.pop();

					switch(ParamIter->second.Type)
					{
						case COMMAND_PARAM_TYPE_STRING:
						default:
							ParamIter->second.Value = CurrentParam;
						break;
						case COMMAND_PARAM_TYPE_INTEGER:
							_set_errno(0);
							strtol(CurrentParam.c_str(), &pConvertTrail, 0);
							_get_errno(&Error);

							ParamIter->second.Value = CurrentParam;

							if (*pConvertTrail != NULL || Error != 0)
							{
								return SetFeedback(PARSER_RESULT_INVALID_INT_PARAMETER, Command_out, ParamsCount,
												   &ParamIter->second, pFeedbackMsg_out, FeedbackMsgSize_out);
							}
						break;
						case COMMAND_PARAM_TYPE_FLOAT:
							_set_errno(0);
							strtod(CurrentParam.c_str(), &pConvertTrail);
							_get_errno(&Error);

							ParamIter->second.Value = CurrentParam;

							if (*pConvertTrail != NULL || Error != 0)
							{
								return SetFeedback(PARSER_RESULT_INVALID_FLOAT_PARAMETER, Command_out, ParamsCount,
												   &ParamIter->second, pFeedbackMsg_out, FeedbackMsgSize_out);
							}
						break;
					}

					++ParamIter;
				}
				while (Params.empty() == false && ParamIter != Command_out.Parameters.end());
			}
		}

		return Result;
	}

	int CommandParser::SetFeedback(int ErrorCode_in, WindowerCommand &Command_in_out, size_t ParamCount_in,
								   const WindowerCommandParam *pCommandParam_in, char **pFeedbackMsg_out,
								   DWORD &MsgSize_out)
	{
		std::string Feedback;

		switch(ErrorCode_in)
		{
			case PARSER_RESULT_INVALID_FLOAT_PARAMETER:
				if (pCommandParam_in != NULL)
				{
					format(Feedback, "Parameter '%s' : invalid float value \u00AB%s\u00BB",
						   pCommandParam_in->Name.c_str(), pCommandParam_in->Value.c_str());
				}
				else
					Feedback = "Invalid float parameter";
			break;
			case PARSER_RESULT_INVALID_INT_PARAMETER:
				if (pCommandParam_in != NULL)
				{
					format(Feedback, "Parameter '%s' : invalid integer value \u00AB%s\u00BB",
						   pCommandParam_in->Name.c_str(), pCommandParam_in->Value.c_str());
				}
				else
					Feedback = "Invalid float parameter";
			break;
			case PARSER_RESULT_TOO_MANY_PARAMETERS:
				format(Feedback, "Too many parameters (%u) : expecting at most %u",
					   ParamCount_in, Command_in_out.MaxParamsCount);
			break;
			case PARSER_RESULT_TOO_FEW_PARAMETERS:
				format(Feedback, "Too few parameters (%u) : expecting at least %u",
					   ParamCount_in, Command_in_out.MinParamsCount);
			break;
			case PARSER_RESULT_IMPLICIT_HELP:
				if (ParamCount_in == 1)
				{
					Command_in_out.Output(Feedback);
				}
				else
				{
					const RegisteredCommands &Commands = m_CommandDispatcher.GetRegisteredCommands();
					RegisteredCommands::const_iterator Iter;

					for (Iter = Commands.begin(); Iter != Commands.end(); ++Iter)
					{
						if (Iter->second != NULL)
						{
							if (Feedback.empty() == false)
								Feedback += '\n';

							Iter->second->Output(Feedback);
						}
					}
				}
			break;
		}

		if (Feedback.empty() == false)
		{
			*pFeedbackMsg_out = _strdup(Feedback.c_str());
			MsgSize_out = Feedback.size();
		}
		// if the code is not for the implicit help command
		if (ErrorCode_in < PARSER_RESULT_IMPLICIT_HELP)
		{
			// invalidate the command
			Command_in_out.Invalidate();
		}

		return ErrorCode_in;
	}

	int CommandParser::Tokenize(const std::string &RawCommand_in, std::string &Command_out, std::queue<std::string> &Params_out)
	{
		std::string::size_type DelimiterPos = 0, SeparatorPos = 0, NotSeparatorPos = 0, LastPos;
		std::string Separator(" "), Delimiter("\"");
		unsigned int ParamsCount = 0;

		SeparatorPos = RawCommand_in.find_first_of(Separator);

		if (SeparatorPos == std::string::npos)
		{
			// no separator found => no parameter
			Command_out = RawCommand_in;
			// if the command is empty => no param found
			if (RawCommand_in.empty() == false)
				++ParamsCount;

			return ParamsCount;
		}
		else
		{
			std::string TmpStr;

			// the first token is the name of the command
			Command_out = RawCommand_in.substr(0, SeparatorPos);
			TmpStr = RawCommand_in.substr(++SeparatorPos);
			++ParamsCount;

			// loop until the command buffer has been consumed or only separators are left
			while (TmpStr.empty() == false && NotSeparatorPos != std::string::npos)
			{
				// find the next character that is not a separator
				NotSeparatorPos = TmpStr.find_first_not_of(Separator);

				if (NotSeparatorPos != std::string::npos)
				{
					// find the next separator in the buffer
					SeparatorPos = TmpStr.find_first_of(Separator, NotSeparatorPos);
					// no separator found => this is the last token
					if (SeparatorPos == std::string::npos)
					{
						Params_out.push(TmpStr.substr(NotSeparatorPos));

						return ++ParamsCount;
					}
					else
					{
						// if the next valid character is not a delimiter => unquoted token
						if (TmpStr[NotSeparatorPos] != Delimiter[0])
						{
							Params_out.push(TmpStr.substr(NotSeparatorPos, SeparatorPos - NotSeparatorPos));
							TmpStr = TmpStr.substr(++SeparatorPos);
							++ParamsCount;
						}
						else
						{
							// the first valid character is the opening delimiter
							DelimiterPos = NotSeparatorPos++;
							LastPos = TmpStr.length() - 1;

							do
							{
								// look for the closing delimiter (delimiter followed by a separator) until
								DelimiterPos = TmpStr.find_first_of(Delimiter, DelimiterPos + 1);
							}
							while (DelimiterPos != std::string::npos			// no delimiter was found or
								&& DelimiterPos != LastPos
								&& TmpStr[DelimiterPos + 1] != Separator[0]);	// a delimiter a closing delimiter was found

							// a closing delimiter was found
							if (DelimiterPos != std::string::npos)
							{
								Params_out.push(TmpStr.substr(NotSeparatorPos, DelimiterPos - NotSeparatorPos));
								TmpStr = TmpStr.substr(++DelimiterPos);
								++ParamsCount;
							}
							// we found a stray delimiter => add it as a parameter
							else
							{
								TmpStr = TmpStr.substr(NotSeparatorPos);
								Params_out.push("" + Delimiter);
								++ParamsCount;
							}
						}
					}
				}
			}

			return ParamsCount;
		}
	}
}