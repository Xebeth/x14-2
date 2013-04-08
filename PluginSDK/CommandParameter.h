/**************************************************************************
	created		:	2011-11-06
	filename	: 	CommandParameter.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower command parameter
**************************************************************************/
#ifndef __COMMAND_PARAMETER_H__
#define __COMMAND_PARAMETER_H__

#ifndef _WINDOWER_PLUGIN_SDK_INCLUDE_
	#error Please include the global header file 'PluginSDK.h'
#endif

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

	//! \brief Windower command parameter
	class CommandParameter
	{
	public:
		CommandParameter(const CommandParameter &CommandParam_in);
		/*! \brief CommandParameter constructor
			\param[in] Type_in : the type of the parameter
			\param[in] Value_in : the value of the parameter
			\param[in] Description_in : the description of the parameter
			\param[in] Name_in : the name of the parameter
		*/
		CommandParameter(COMMAND_PARAM_TYPE Type_in, bool bOptional_in, const std::string& Value_in,
						 const std::string& Description_in, const std::string& Name_in)
			: m_Type(Type_in), m_bOptional(bOptional_in), m_Value(Value_in),
			  m_Name(Name_in), m_Description(Description_in) {}

		/*! \brief Retrieves the string value of the parameter
			\return the string value of the parameter
		*/
		const std::string& GetStringValue() const { return m_Value; }
		/*! \brief Sets the string value of the parameter
			\param[in] StringValue_in : the string value of the parameter
		*/
		void SetStringValue(const std::string& StringValue_in) { m_Value = StringValue_in; }

		/*! \brief Retrieves the pointer value of the parameter
			\return the pointer value of the parameter
		*/
		long GetPointerValue() const \
		{ return strtol(m_Value.c_str(), NULL, 16); }
		/*! \brief Sets the pointer value of the parameter
			\param[in] PointerValue_in : the pointer value of the parameter
		*/
		void SetPointerValue(long PointerValue_in) \
		{ format(m_Value, "%08x", PointerValue_in); }

		/*! \brief Retrieves the integer value of the parameter
			\return the integer value of the parameter
		*/
		long GetIntegerValue() const \
		{ return strtol(m_Value.c_str(), NULL, 10); }
		/*! \brief Sets the integer value of the parameter
			\param[in] IntegerValue_in : the integer value of the parameter
		*/
		void SetIntegerValue(long IntegerValue_in) \
		{ format(m_Value, "%ld", IntegerValue_in); }

		/*! \brief Retrieves the float value of the parameter
			\return the float value of the parameter
		*/
		double GetFloatValue() const \
		{ return strtod(m_Value.c_str(), NULL); }
		/*! \brief Sets the float value of the parameter
			\param[in] FloatValue_in : the float value of the parameter
		*/
		void SetFloatValue(double FloatValue_in) \
		{ format(m_Value, "%g", FloatValue_in); }

		/*! \brief Retrieves the type of the parameter
			\return the type of the parameter
			\sa COMMAND_PARAM_TYPE
		*/
		COMMAND_PARAM_TYPE GetType() const { return m_Type; }
		/*! \brief Retrieves the name of the parameter
			\return the name of the parameter
		*/
		const std::string& GetName() const { return m_Name; }
		/*! \brief Retrieves the description of the parameter
			\return the description of the parameter
		*/
		const std::string& GetDescription() const { return m_Description; }
		/*! \brief Checks if the parameter is optional
			\return true if the parameter is optional; false otherwise
		*/
		bool IsOptional() const { return m_bOptional; }
		/*! \brief Sets the flag specifying if the parameter is optional
			\param[in] bOptional_in : the new value of the flag
		*/
		void SetOptional(bool bOptional_in = false) { m_bOptional = bOptional_in; }

	protected:
		//! flag specifying if the parameter is optional
		bool m_bOptional;
		//! the type of the parameter
		COMMAND_PARAM_TYPE m_Type;
		//! the description of the parameter
		std::string m_Description;
		//! the value of the parameter
		std::string m_Value;
		//! the name of the parameter
		std::string m_Name;
	};

	//! \brief Parameter used to revoke a command registration
	class UnregisterParam
	{
	public:
		//! \brief UnregisterParam default constructor
		UnregisterParam() : RegistrationKey(0) {}
		/*! \brief UnregisterParam constructor
			\param[in] RegistrationKey_in : the key of the plugin revoking the command registration
			\param[in] CommandName_in : the ID of the command to unregister
		*/
		UnregisterParam(DWORD RegistrationKey_in, const std::string& CommandName_in)
			: RegistrationKey(RegistrationKey_in), CommandName(CommandName_in) {}

		//! the name of the command to unregister
		std::string CommandName;
		//! the key of the plugin revoking the command registration
		DWORD RegistrationKey;
	};
}

#endif//__COMMAND_PARAMETER_H__