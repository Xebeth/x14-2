/**************************************************************************
	created		:	2011-06-07
	filename	: 	WindowerCommand.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower command and related structures
**************************************************************************/
#ifndef __WINDOWER_COMMAND_H__
#define __WINDOWER_COMMAND_H__

#include "utf8_convert.h"

namespace Windower
{
	//! types of command parameters
	enum COMMAND_PARAM_TYPE
	{
		COMMAND_PARAM_TYPE_STRING = 0,	//!< string
		COMMAND_PARAM_TYPE_POINTER,		//!< pointer
		COMMAND_PARAM_TYPE_INTEGER,		//!< integer
		COMMAND_PARAM_TYPE_FLOAT,		//!< float
		COMMAND_PARAM_TYPE_COUNT		//!< number of types
	};

	//! result codes of command dispatching
	enum DISPATCHER_RESULT
	{
		DISPATCHER_RESULT_INVALID_CALL = -2,	//!< invalid call
		DISPATCHER_RESULT_INVALID_PARAMETERS,	//!< invalid parameters
		DISPATCHER_RESULT_SUCCESS				//!< success
	};

	//! \brief Command calling parameter
	class CallerParam
	{
	public:
		//! \brief CallerParam default constructor
		CallerParam() : DataType("NULL"), pData(NULL) {}
		/*! \brief CallerParam constructor
			\param[in] DataType_in : a string description the type of calling object
			\param[in] pData_in : a pointer to the calling object
		*/
		CallerParam(std::string DataType_in, LPVOID pData_in)
		{
			DataType = DataType_in;
			pData = pData_in;
		}
		//! a string description the type of calling object
		std::string	DataType;
		//! a pointer to the calling object
		LPVOID		pData;
	};

	class WindowerCommand;
	//! function pointer to a command callback
	typedef int (*fnCommandCallback)(const WindowerCommand*);

	//! \brief Windower command parameter
	class WindowerCommandParam
	{
	public:
		//! \brief WindowerCommandParam default constructor
		WindowerCommandParam() : Type(COMMAND_PARAM_TYPE_STRING) {}
		/*! \brief WindowerCommandParam constructor
			\param[in] Type_in : the type of the parameter
			\param[in] Value_in : the value of the parameter
			\param[in] Description_in : the description of the parameter
			\param[in] Name_in : the name of the parameter
		*/
		WindowerCommandParam(COMMAND_PARAM_TYPE Type_in, const std::string &Value_in,
							 const std::string &Description_in, const std::string &Name_in)
			: Type(Type_in), Value(Value_in), Name(Name_in), Description(Description_in) {}

		//! the type of the parameter
		COMMAND_PARAM_TYPE Type;
		//! the description of the parameter
		std::string Description;
		//! the value of the parameter
		std::string Value;
		//! the name of the parameter
		std::string Name;

		/*! \brief Retrieves the integer value of a parameter
			\return the integer value of a parameter
		*/
		long GetIntegerValue() const
		{
			return strtol(Value.c_str(), NULL, 0);
		}
		/*! \brief Retrieves the float value of a parameter
			\return the float value of a parameter
		*/
		double GetFloatValue() const
		{
			return strtod(Value.c_str(), NULL);
		}
		/*! \brief Retrieves the integer value of a parameter formatted as hexadecimal
			\return the pointer value of a parameter
		*/
		long GetPointerValue() const
		{
			long Pointer = NULL;

			sscanf_s(Value.c_str(), "%08x", &Pointer);

			return Pointer;
		}
		/*! \brief Retrieves the string value of a parameter converted to UTF-8
			\return the string value of a parameter converted to UTF-8
		*/
		string_t& GetWideStringValue(string_t &Result_out) const
		{
			return convert_utf8(Value, Result_out);
		}
	};

	//! a hash map of command parameters
	typedef stdext::hash_map<std::string, WindowerCommandParam> CommandParameters;

	//! \brief Windower command
	class WindowerCommand
	{
	public:
		WindowerCommand();
		/*! \brief WindowerCommand constructor
			\param[in] RegistrationKey_in : the key of the plugin registering the command
			\param[in] Name_in : the name of the command
			\param[in] Description_in : the description of the command
			\param[in] Caller_in : the caller parameters (typically a plugin instance)
			\param[in] CallbackFunc_in : function pointer to call when the command is invoked
			\param[in] MinParamsCount_in : minimal number of parameters for the command
			\param[in] MaxParamsCount_in : maximal number of parameters for the command
			\param[in] Parameters_in : a collection of parameters
			\param[in] Public_in : flag specifying if the command is listed with //help
			\param[in] Restricted_in : flag specifying if the command can be called by the user
			\return true if the command was registered successfully; false otherwise
		*/
		WindowerCommand(unsigned long RegistrationKey_in, const std::string &Name_in,
						const std::string &Description_in, CallerParam Caller_in,
						fnCommandCallback CallbackFunc_in, const CommandParameters &Parameters_in,
						unsigned int MinParamsCount_in = 0U, unsigned int MaxParamsCount_in = 0U,
						bool Public_in = true, bool Restricted_in = false)
			: CommandCallbackFunc(CallbackFunc_in), Restricted(Restricted_in), Description(Description_in),
			  Public(Public_in), Caller(Caller_in), Name(Name_in), MinParamsCount(MinParamsCount_in),
			  MaxParamsCount(MaxParamsCount_in), Parameters(Parameters_in), RegistrationKey(RegistrationKey_in) {}

		/*! \brief Retrieves a parameter given its name
			\param[in] ParamName_in : the name of the parameter
			\return a pointer to the parameter if found; NULL otherwise
		*/
		inline const WindowerCommandParam* GetParameter(const std::string &ParamName_in) const
		{
			CommandParameters::const_iterator Iter = Parameters.find(ParamName_in);

			if (Iter != Parameters.end())
				return &Iter->second;

			return NULL;
		}

		std::string& Output(std::string &Output_out, bool ShowValues_in = false);
		void Invalidate();

		static char* CommandParamTypes[COMMAND_PARAM_TYPE_COUNT];

		//! flag specifying if the command is listed with //help
		bool Public;
		//! flag specifying if the command can be called by the user
		bool Restricted;
		//! the result of the command once invoked
		mutable std::string ResultMsg;
		//! the name of the command
		std::string Name;
		//! the description of the command
		std::string Description;
		//! minimal number of parameters for the command
		unsigned int MinParamsCount;
		//! maximal number of parameters for the command
		unsigned int MaxParamsCount;
		//! the key of the plugin registering the command
		unsigned long RegistrationKey;
		//! the caller parameters (typically a plugin instance)
		CallerParam Caller;
		//! a collection of parameters
		CommandParameters Parameters;
		//! function pointer to call when the command is invoked
		fnCommandCallback CommandCallbackFunc;
	};

	//! \brief Parameter used to revoke a command registration
	class UnregisterParam
	{
	public:
		//! \brief UnregisterParam default constructor
		UnregisterParam() : RegistrationKey(0) {}
		/*! \brief UnregisterParam constructor
			\param[in] RegistrationKey_in : the key of the plugin revoking the command registration
			\param[in] CommandName_in : the name of the command to unregister
		*/
		UnregisterParam(unsigned long RegistrationKey_in, const std::string &CommandName_in)
			: RegistrationKey(RegistrationKey_in), CommandName(CommandName_in) {}

		//! the name of the command to unregister
		std::string		CommandName;
		//! the key of the plugin revoking the command registration
		unsigned long	RegistrationKey;
	};
}

#endif//__WINDOWER_COMMAND_H__