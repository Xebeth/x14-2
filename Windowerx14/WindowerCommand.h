/**************************************************************************
	created		:	2011-06-07
	filename	: 	WindowerCommand.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#ifndef __WINDOWER_COMMAND_H__
#define __WINDOWER_COMMAND_H__

#include "utf8_convert.h"

namespace Windower
{
	enum COMMAND_PARAM_TYPE
	{
		COMMAND_PARAM_TYPE_STRING = 0,
		COMMAND_PARAM_TYPE_INTEGER,
		COMMAND_PARAM_TYPE_FLOAT,
		COMMAND_PARAM_TYPE_COUNT
	};

	enum DISPATCHER_RESULT
	{
		DISPATCHER_RESULT_INVALID_CALL = -2,
		DISPATCHER_RESULT_DEFAULT_FUNC_INVOKED,
		DISPATCHER_RESULT_SUCCESS
	};

	class CallerParam
	{
	public:
		CallerParam() : DataType("NULL"), pData(NULL) {}

		CallerParam(std::string DataType_in, void *pData_in)
		{
			DataType = DataType_in;
			pData = pData_in;
		}

		std::string	DataType;
		void*		pData;
	};

	class WindowerCommand;

	typedef int (*fnCommandCallback)(const WindowerCommand*);

	class WindowerCommandParam
	{
	public:
		WindowerCommandParam() : Type(COMMAND_PARAM_TYPE_STRING) {}
		WindowerCommandParam(COMMAND_PARAM_TYPE Type_in, const std::string &Value_in,
							 const std::string &Description_in, const std::string &Name_in)
			: Type(Type_in), Value(Value_in), Name(Name_in), Description(Description_in) {}

		COMMAND_PARAM_TYPE Type;
		std::string Description;
		std::string Value;
		std::string Name;

		long GetIntegerValue() const
		{
			return strtol(Value.c_str(), NULL, 0);
		}

		double GetFloatValue() const
		{
			return strtod(Value.c_str(), NULL);
		}

		string_t& GetWideStringValue(string_t &Result_out) const
		{
			return convert_utf8(Value, Result_out);
		}
	};

	typedef stdext::hash_map<std::string, WindowerCommandParam> CommandParameters;

	class WindowerCommand
	{
	public:
		WindowerCommand(unsigned long RegistrationKey_in, const std::string &Name_in,
						const std::string &Description_in, CallerParam Caller_in,
						fnCommandCallback CallbackFunc_in, const CommandParameters &Parameters_in,
						unsigned int MinParamsCount_in, unsigned int MaxParamsCount_in,
						bool Public_in = true, bool Restricted_in = false)
			: CommandCallbackFunc(CallbackFunc_in), Restricted(Restricted_in), Description(Description_in),
			  Public(Public_in), Caller(Caller_in), Name(Name_in), MinParamsCount(MinParamsCount_in),
			  MaxParamsCount(MaxParamsCount_in), Parameters(Parameters_in), RegistrationKey(RegistrationKey_in) {}
		WindowerCommand();

		inline const WindowerCommandParam* GetParameter(const std::string &ParamName_in) const
		{
			CommandParameters::const_iterator Iter = Parameters.find(ParamName_in);

			if (Iter != Parameters.end())
				return &Iter->second;
			else
				return NULL;
		}

		std::string& Output(std::string &Output_out, bool ShowValues_in = false);
		void Invalidate();

		static int DefaultFunc(const WindowerCommand *pCommand_in);
		static char* CommandParamTypes[COMMAND_PARAM_TYPE_COUNT];

		bool Public;
		bool Restricted;
		std::string Name;
		std::string Description;
		unsigned int MinParamsCount;
		unsigned int MaxParamsCount;
		unsigned long RegistrationKey;
		// command execution parameters
		CallerParam Caller;
		CommandParameters Parameters;
		fnCommandCallback CommandCallbackFunc;
	};

	typedef struct _UnregisterParam
	{
		_UnregisterParam() : RegistrationKey(0) {}
		_UnregisterParam(unsigned long RegistrationKey_in, const std::string &CommandName_in)
			: RegistrationKey(RegistrationKey_in), CommandName(CommandName_in) {}

		std::string		CommandName;
		unsigned long	RegistrationKey;
	} UnregisterParam;
}

#endif//__WINDOWER_COMMAND_H__