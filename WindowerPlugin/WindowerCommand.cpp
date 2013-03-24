/**************************************************************************
	created		:	2011-06-07
	filename	: 	WindowerCommand.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>

#include "WindowerCommand.h"
#include "CommandHandler.h"

namespace Windower
{
	//! strings representing the parameter types
	char* WindowerCommand::CommandParamTypes[COMMAND_PARAM_TYPE_COUNT] =
	{
		"string",	// string
		"pointer",	// pointer
		"integer",	// integer
		"float"		// float
	};

	//! \brief WindowerCommand default constructor
	WindowerCommand::WindowerCommand() : m_RefCount(0U)
	{
		Invalidate();
	}

	/*! \brief Copies the specified command
		\param[in] Command_in : the command to copy
	*/
	void WindowerCommand::Copy(const WindowerCommand &Command_in)
	{
		m_MinParamsCount = Command_in.m_MinParamsCount;
		m_MaxParamsCount = Command_in.m_MaxParamsCount;
		m_Description = Command_in.m_Description;		
		m_Restricted = Command_in.m_Restricted;
		m_pHandler = Command_in.m_pHandler;
		m_Public = Command_in.m_Public;
		m_Name = Command_in.m_Name;
		m_ID = Command_in.m_ID;

		CommandParams::const_iterator ParamIt;

		for (ParamIt = Command_in.m_Parameters.begin(); ParamIt != Command_in.m_Parameters.end(); ++ParamIt)
			m_Parameters[ParamIt->first] = new CommandParameter(*ParamIt->second);
	}

	/*! \brief Formats the help message for the command
		\param[out] Help_out : a string receiving the formatted help message
		\param[in] ShowValues_in : flag specifying if the description or value is displayed
		\return the formatted help message
	*/
	std::string& WindowerCommand::Output(std::string &Help_out, bool ShowValues_in)
	{
		if (m_Public && m_Parameters.empty() == false)
		{
			size_t ParamCount = 0, ParamSize = m_Parameters.size();
			CommandParams::const_iterator Iter;
			std::string ParamDesc, ValueDesc;
			char *pParamFormat = NULL;

			append_format(Help_out, "\xe2\x87\x92 %s", m_Name.c_str());

			if (m_Description.empty() == false)
				Help_out.append(" : " + m_Description);

			Help_out += '\n';

			// if the descriptions aren't empty, their keys must match the parameter type ones
			for(Iter = m_Parameters.begin(); Iter != m_Parameters.end(); ++Iter)
			{
				if (Iter->second != NULL)
				{
					if (ShowValues_in)
						ValueDesc = Iter->second->GetStringValue();
					else
						ValueDesc = Iter->second->GetDescription();

					if (ValueDesc.empty() == false || ShowValues_in)
					{
						if (ParamCount < m_MinParamsCount - 1)
							pParamFormat = "\t[%s <%s>]\t: %s";
						else
							pParamFormat = "\t%s <%s>\t: %s";

						format(ParamDesc, pParamFormat, CommandParamTypes[Iter->second->GetType()],
							   Iter->second->GetName().c_str(), ValueDesc.c_str());
					}
					else
					{
						if (ParamCount < m_MinParamsCount - 1)
							pParamFormat = "\t[%s <%s>]";
						else
							pParamFormat = "\t%s <%s>";

						format(ParamDesc, pParamFormat, CommandParamTypes[Iter->second->GetType()],
							   Iter->second->GetName().c_str());
					}

					Help_out.append(ParamDesc);

					if (++ParamCount < ParamSize)
						Help_out += '\n';
				}
			}
		}
		else if (m_Description.empty() == false)
		{
			append_format(Help_out, "\xe2\x87\x92 %s : %s", m_Name.c_str(), m_Description.c_str());
		}
		else
		{
			append_format(Help_out, "No help available for command '%s'", m_Name.c_str());
		}

		return Help_out;
	}

	//! \brief Sets a command to its default state
	void WindowerCommand::Invalidate()
	{
		m_MinParamsCount = m_MaxParamsCount = 0U;
		m_Name = "%empty%";
		m_Description = "";
		m_RegistrationKey = 0UL;
		m_Parameters.clear();
		m_Restricted = true;
		m_Public = false;
		m_pHandler = NULL;
		m_ID = -1;
	}

	/*! \brief Executes the command by invoking its callback
		\param[out] Feedback_out : the result message of the execution
		\return DISPATCHER_RESULT_SUCCESS if successful; an error code otherwise
	*/
	int WindowerCommand::Execute(std::string &Feedback_out)
	{
		if (m_pHandler != NULL)
			return m_pHandler->DefaultHandler(this, Feedback_out);
		else
			format(Feedback_out, "The command '%s' is not associated with any action.", m_Name.c_str());

		return DISPATCHER_RESULT_INVALID_CALL;
	}

	/*! \brief Retrieves the string value of a parameter
		\param[in] Name_in : the name of the parameter
		\return the string value of a parameter
	*/
	const std::string WindowerCommand::GetStringValue(const std::string &Name_in) const
	{
		CommandParameter *pParam = GetParameter(Name_in);

		if (pParam != NULL)
			return pParam->GetStringValue();

		return "";
	}

	/*! \brief Retrieves the string value of a parameter as unicode
		\param[in] Name_in : the name of the parameter
		\return the string value of a parameter as unicode
	*/
	const string_t WindowerCommand::GetWideStringValue(const std::string &Name_in) const
	{
		CommandParameter *pParam = GetParameter(Name_in);
		string_t Result;

		if (pParam != NULL)
			convert_utf8(pParam->GetStringValue(), Result);

		return Result;
	}

	/*! \brief Sets the string value of a parameter
		\param[in] Name_in : the name of the parameter
		\param[in] pDefaultValue_in : the string value of a parameter
		\return true if the parameter value was set; false otherwise
	*/
	bool WindowerCommand::SetStringValue(const std::string &Name_in, const char *pDefaultValue_in)
	{
		std::string Value = (pDefaultValue_in != NULL) ? pDefaultValue_in : "";
		CommandParameter *pParam = GetParameter(Name_in);
		
		if (pParam != NULL)
		{
			pParam->SetStringValue(Value);

			return true;
		}

		return false;
	}

	/*! \brief Retrieves the integer value of a parameter formatted as hexadecimal
		\param[in] Name_in : the name of the parameter
		\return the pointer value of a parameter
	*/
	long WindowerCommand::GetPointerValue(const std::string &Name_in) const
	{
		CommandParameter *pParam = GetParameter(Name_in);

		if (pParam != NULL)
			return pParam->GetPointerValue();

		return 0L;
	}

	/*! \brief Sets the pointer value of a parameter
		\param[in] Name_in : the name of the parameter
		\param[in] DefaultValue_in : the pointer value of a parameter
		\return true if the parameter value was set; false otherwise
	*/
	bool WindowerCommand::SetPointerValue(const std::string &Name_in, long DefaultValue_in)
	{
		CommandParameter *pParam = GetParameter(Name_in);

		if (pParam != NULL)
		{
			pParam->SetPointerValue(DefaultValue_in);

			return true;
		}

		return false;
	}
	
	/*! \brief Retrieves the integer value of a parameter
		\param[in] Name_in : the name of the parameter
		\return the integer value of a parameter
	*/
	long WindowerCommand::GetIntegerValue(const std::string &Name_in) const
	{
		CommandParameter *pParam = GetParameter(Name_in);

		if (pParam != NULL)
			return pParam->GetIntegerValue();

		return 0L;
	}

	/*! \brief Sets the integer value of a parameter
		\param[in] Name_in : the name of the parameter
		\param[in] DefaultValue_in : the integer value of a parameter
		\return true if the parameter value was set; false otherwise
	*/
	bool WindowerCommand::SetIntegerValue(const std::string &Name_in, long DefaultValue_in)
	{
		CommandParameter *pParam = GetParameter(Name_in);

		if (pParam != NULL)
		{
			pParam->SetIntegerValue(DefaultValue_in);

			return true;
		}

		return false;
	}

	/*! \brief Retrieves the float value of a parameter
		\param[in] Name_in : the name of the parameter
		\return the float value of a parameter
	*/
	double WindowerCommand::GetFloatValue(const std::string &Name_in) const
	{
		CommandParameter *pParam = GetParameter(Name_in);

		if (pParam != NULL)
			return pParam->GetFloatValue();

		return 0.;
	}

	/*! \brief Sets the float value of a parameter
		\param[in] Name_in : the name of the parameter
		\param[in] DefaultValue_in : the float value of a parameter
		\return true if the parameter value was set; false otherwise
	*/
	bool WindowerCommand::SetFloatValue(const std::string &Name_in, double DefaultValue_in)
	{
		CommandParameter *pParam = GetParameter(Name_in);

		if (pParam != NULL)
		{
			pParam->SetFloatValue(DefaultValue_in);

			return true;
		}

		return false;
	}

	//! \brief Removes all the parameters
	void WindowerCommand::Clear()	
	{
		CommandParams::iterator ParamIt;

		for (ParamIt = m_Parameters.begin(); ParamIt != m_Parameters.end(); ++ParamIt)
			delete ParamIt->second;

		Invalidate();
	}
	
	/*! \brief Retrieves a parameter given its name
		\param[in] Name_in : the name of the parameter
		\return a pointer to the parameter if it exists; NULL otherwise
	*/
	CommandParameter* WindowerCommand::GetParameter(const std::string &Name_in) const
	{
		CommandParams::const_iterator ParamIt = m_Parameters.find(Name_in);

		if (ParamIt != m_Parameters.end())
			return ParamIt->second;

		return NULL;
	}

	/*! \brief Creates a parameter of the specified type
		\param[in] Name_in : the name of the parameter
		\param[in] bOptional_in : flag specifying if the parameter is optional
		\param[in] Desc_in : the description of the parameter
		\param[in] Type_in : the type of the parameter
		\return true a pointer to the parameter
	*/
	CommandParameter* WindowerCommand::CreateParameter(const std::string &Name_in,
													   bool bOptional_in,
													   const std::string &Desc_in,
													   COMMAND_PARAM_TYPE Type_in)
	{
		CommandParameter *pParam = GetParameter(Name_in);

		// check if the parameter doesn't exist yet
		if (pParam == NULL)
		{
			if (bOptional_in == false)
			{
				++m_MaxParamsCount;
				++m_MinParamsCount;
			}
			else
				++m_MaxParamsCount;

			return (m_Parameters[Name_in] = new CommandParameter(Type_in, bOptional_in, "", Desc_in, Name_in));
		}
		else if (pParam->GetType() == Type_in)
			return pParam;

		return NULL;
	}

	/*! \brief Creates a parameter of the type string
		\param[in] Name_in : the name of the parameter
		\param[in] bOptional_in : flag specifying if the parameter is optional
		\param[in] pDefaultValue_in : the value of the parameter
		\param[in] pDesc_in : the description of the parameter
		\return true if the parameter was created successfully; false otherwise
	*/
	bool WindowerCommand::AddStringParam(const std::string &Name_in, bool bOptional_in,
									   const char *pDefaultValue_in, const char *pDesc_in)
	{
		std::string Desc = (pDesc_in != NULL) ? pDesc_in : "";
		CommandParameter *pNewParam;

		pNewParam = CreateParameter(Name_in, bOptional_in, Desc, COMMAND_PARAM_TYPE_STRING);

		if (pNewParam != NULL)
		{
			std::string Value = (pDefaultValue_in != NULL) ? pDefaultValue_in : "";

			pNewParam->SetStringValue(Value);

			return true;
		}

		return false;
	}

	/*! \brief Creates a parameter of the type pointer
		\param[in] Name_in : the name of the parameter
		\param[in] bOptional_in : flag specifying if the parameter is optional
		\param[in] pDefaultValue_in : the value of the parameter
		\param[in] pDesc_in : the description of the parameter
		\return true if the parameter was created successfully; false otherwise
	*/
	bool WindowerCommand::AddPointerParam(const std::string &Name_in, bool bOptional,
										long DefaultValue_in, const char *pDesc_in)
	{
		std::string Desc = (pDesc_in != NULL) ? pDesc_in : "";
		CommandParameter *pNewParam;

		pNewParam = CreateParameter(Name_in, bOptional, Desc, COMMAND_PARAM_TYPE_POINTER);

		if (pNewParam != NULL)
		{
			pNewParam->SetPointerValue(DefaultValue_in);

			return true;
		}

		return false;
	}

	/*! \brief Creates a parameter of the type integer
		\param[in] Name_in : the name of the parameter
		\param[in] bOptional_in : flag specifying if the parameter is optional
		\param[in] pDefaultValue_in : the value of the parameter
		\param[in] pDesc_in : the description of the parameter
		\return true if the parameter was created successfully; false otherwise
	*/
	bool WindowerCommand::AddIntegerParam(const std::string &Name_in, bool bOptional,
										long DefaultValue_in, const char *pDesc_in)
	{
		std::string Desc = (pDesc_in != NULL) ? pDesc_in : "";
		CommandParameter *pNewParam;

		pNewParam = CreateParameter(Name_in, bOptional, Desc, COMMAND_PARAM_TYPE_POINTER);

		if (pNewParam != NULL)
		{
			pNewParam->SetIntegerValue(DefaultValue_in);

			return true;
		}

		return false;
	}

	/*! \brief Creates a parameter of the type float
		\param[in] Name_in : the name of the parameter
		\param[in] bOptional_in : flag specifying if the parameter is optional
		\param[in] pDefaultValue_in : the value of the parameter
		\param[in] pDesc_in : the description of the parameter
		\return true if the parameter was created successfully; false otherwise
	*/
	bool WindowerCommand::AddFloatParam(const std::string &Name_in, bool bOptional,
									  double DefaultValue_in, const char *pDesc_in)
	{
		std::string Desc = (pDesc_in != NULL) ? pDesc_in : "";
		CommandParameter *pNewParam;

		pNewParam = CreateParameter(Name_in, bOptional, Desc, COMMAND_PARAM_TYPE_FLOAT);

		if (pNewParam != NULL)
		{
			pNewParam->SetFloatValue(DefaultValue_in);

			return true;
		}

		return false;
	}
}