/**************************************************************************
	created		:	2011-06-06
	filename	: 	CommandParser.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Command parser
**************************************************************************/
#ifndef __COMMAND_PARSER_H__
#define __COMMAND_PARSER_H__

namespace Windower
{
	class CommandParser : public WindowerCore
	{
	public:
		enum PARSER_RESULT
		{
			PARSER_RESULT_INVALID_FLOAT_PARAMETER = -6,
			PARSER_RESULT_INVALID_INT_PARAMETER,
			PARSER_RESULT_TOO_MANY_PARAMETERS,
			PARSER_RESULT_TOO_FEW_PARAMETERS,
			PARSER_RESULT_INVALID_COMMAND,
			PARSER_RESULT_IMPLICIT_HELP
		};

		CommandParser(WindowerEngine &Engine_in, CommandDispatcher &Dispatcher_in);

		// ICoreModule interface implementation
		void RegisterHooks(IHookManager *pHookManager){}
		void OnHookInstall(IHookManager *pHookManager){}

		int ParseCommand(const char *pRawCommand_in, WindowerCommand &Command_out,
						 char **pFeedbackMsg_out, DWORD &FeedbackMsgSize_out);

	protected:
		int Tokenize(const std::string &RawCommand_in, std::string &Command_out, std::queue<std::string> &Params_out);
		int SetFeedback(int Code_in, WindowerCommand &Command_in_out, size_t ParamCount_in,
						const WindowerCommandParam *pCommandParam_in, char **pFeedbackMsg_out,
						DWORD &MsgSize_out);

		CommandDispatcher &m_CommandDispatcher;
	};
}

#endif//__COMMAND_PARSER_H__