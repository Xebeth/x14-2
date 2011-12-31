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
	//! \brief Command parser
	class CommandParser : public WindowerCore
	{
	public:
		//! Result of command parsing
		enum PARSER_RESULT
		{
			PARSER_RESULT_INVALID_FLOAT_PARAMETER = 0,
			PARSER_RESULT_INVALID_INT_PARAMETER,
			PARSER_RESULT_TOO_MANY_PARAMETERS,
			PARSER_RESULT_TOO_FEW_PARAMETERS,
			PARSER_RESULT_INVALID_COMMAND
		};

		CommandParser(WindowerEngine &Engine_in_out, CommandDispatcher &Dispatcher_in);

		// ICoreModule interface implementation
		/*! \brief Register the hooks for this module
			\param[in] HookManager_in : the hook manager
		*/
		void RegisterHooks(IHookManager &HookManager_in) {}
		/*! \brief Callback invoked when the hooks of the module are installed
			\param[in] HookManager_in : the hook manager
		*/
		void OnHookInstall(IHookManager &HookManager_in) {}

		int ParseCommand(const char *pRawCommand_in, WindowerCommand &Command_out,
						 char **pFeedbackMsg_out, DWORD &FeedbackMsgSize_out);

	protected:
		int Tokenize(const std::string &RawCommand_in, std::string &Command_out, std::queue<std::string> &Params_out);
		int SetFeedback(int Code_in, WindowerCommand &Command_in_out, size_t ParamCount_in,
						const char *pParamName_in, const char *pParamValue_in,
						char **pFeedbackMsg_out, DWORD &MsgSize_out);

		//! the command dispatcher
		CommandDispatcher &m_CommandDispatcher;
	};
}

#endif//__COMMAND_PARSER_H__