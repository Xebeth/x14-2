/**************************************************************************
	created		:	2011-11-06
	filename	: 	CommandParameter.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "CommandParameter.h"

namespace Windower
{
	UnregisterParam::UnregisterParam()
		: RegistrationKey(0UL) {}

	UnregisterParam::UnregisterParam(DWORD RegistrationKey_in, const std::string& CommandName_in)
		: RegistrationKey(RegistrationKey_in), CommandName(CommandName_in) {}

	CommandParameter::CommandParameter(const CommandParameter &CommandParam_in)
	{
		m_Description = CommandParam_in.m_Description;
		m_Type = CommandParam_in.m_Type;
		m_Name = CommandParam_in.m_Name;		
	}

	CommandParameter::CommandParameter(COMMAND_PARAM_TYPE Type_in, bool bOptional_in, const std::string& Value_in,
									   const std::string& Description_in, const std::string& Name_in)
		: m_Type(Type_in), m_bOptional(bOptional_in), m_Value(Value_in),
		  m_Name(Name_in), m_Description(Description_in) {}

	const std::string& CommandParameter::GetStringValue() const
	{ return m_Value; }
	void CommandParameter::SetStringValue(const std::string& StringValue_in)
	{ m_Value = StringValue_in; }

	long CommandParameter::GetPointerValue() const
	{ return strtol(m_Value.c_str(), NULL, 16); }
	void CommandParameter::SetPointerValue(long PointerValue_in)
	{ format(m_Value, "%08x", PointerValue_in); }

	long CommandParameter::GetIntegerValue() const
	{ return strtol(m_Value.c_str(), NULL, 10); }
	void CommandParameter::SetIntegerValue(long IntegerValue_in)
	{ format(m_Value, "%ld", IntegerValue_in); }

	double CommandParameter::GetFloatValue() const
	{ return strtod(m_Value.c_str(), NULL); }
	void CommandParameter::SetFloatValue(double FloatValue_in)
	{ format(m_Value, "%g", FloatValue_in); }

	Windower::COMMAND_PARAM_TYPE CommandParameter::GetType() const
	{ return m_Type; }

	const std::string& CommandParameter::GetName() const
	{ return m_Name; }

	const std::string& CommandParameter::GetDescription() const
	{ return m_Description; }

	bool CommandParameter::IsOptional() const
	{ return m_bOptional; }
	void CommandParameter::SetOptional(bool bOptional_in)
	{ m_bOptional = bOptional_in; }
}