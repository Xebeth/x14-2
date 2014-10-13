/**************************************************************************
	created		:	2011-06-07
	filename	: 	WindowerCommand.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "CommandParameter.h"
#include "ICommandHandler.h"
#include "WindowerCommand.h"

namespace Windower
{
	char* WindowerCommand::CommandParamTypes[COMMAND_PARAM_TYPE_COUNT] =
	{
		"string",	// string
		"pointer",	// pointer
		"integer",	// integer
		"float"		// float
	};

	WindowerCommand::WindowerCommand()
		: m_RefCount(0U) { Invalidate(); }

	WindowerCommand::WindowerCommand(const WindowerCommand &Command_in)
	{ Copy(Command_in); }

	WindowerCommand::WindowerCommand(DWORD RegistrationKey_in, INT_PTR CmdID_in, const std::string &Name_in,
									 const std::string &Description_in, ICommandHandler *pHandler_in,
									 bool Public_in, bool Restricted_in)
		: m_Description(Description_in), m_ID(CmdID_in), m_Name(Name_in), m_Restricted(Restricted_in), 
		  m_MinParamsCount(0U), m_MaxParamsCount(0U), m_Public(Public_in), m_pHandler(pHandler_in),
		  m_RegistrationKey(RegistrationKey_in), m_RefCount(0U) {}

	WindowerCommand::~WindowerCommand() { Clear(); }

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

		CommandParams::const_iterator ParamIt, EndIt = Command_in.m_Parameters.cend();

		for (ParamIt = Command_in.m_Parameters.cbegin(); ParamIt != EndIt; ++ParamIt)
			m_Parameters[ParamIt->first] = new CommandParameter(*ParamIt->second);
	}

	std::string& WindowerCommand::Output(std::string &Help_out, bool ShowValues_in)
	{
		if (m_Public && m_Parameters.empty() == false)
		{
			CommandParams::const_iterator ParamIt, EndIt = m_Parameters.cend();
			size_t ParamCount = 0, ParamSize = m_Parameters.size();			
			std::string ParamDesc, ValueDesc;
			char *pParamFormat = NULL;

			append_format(Help_out, "\xee\x81\xaf %s", m_Name.c_str());

			if (m_Description.empty() == false)
				Help_out.append(" : " + m_Description);

			Help_out += '\n';

			// if the descriptions aren't empty, their keys must match the parameter type ones
			for(ParamIt = m_Parameters.cbegin(); ParamIt != EndIt; ++ParamIt)
			{
				if (ParamIt->second != NULL)
				{
					if (ShowValues_in)
						ValueDesc = ParamIt->second->GetStringValue();
					else
						ValueDesc = ParamIt->second->GetDescription();

					if (ValueDesc.empty() == false || ShowValues_in)
					{
						if (ParamCount < m_MinParamsCount - 1)
							pParamFormat = "     - [%s <%s>] : %s";
						else
							pParamFormat = "     - %s <%s> : %s";

						format(ParamDesc, pParamFormat, ParamIt->second->GetName().c_str(),
							   CommandParamTypes[ParamIt->second->GetType()], ValueDesc.c_str());
					}
					else
					{
						if (ParamCount < m_MinParamsCount - 1)
							pParamFormat = "     - [%s <%s>]";
						else
							pParamFormat = "     - %s <%s>";

						format(ParamDesc, pParamFormat, ParamIt->second->GetName().c_str(),
							   CommandParamTypes[ParamIt->second->GetType()]);
					}

					if (ParamDesc.empty() == false)
						Help_out.append("   Parameters:\n" + ParamDesc);

					if (++ParamCount < ParamSize)
						Help_out += '\n';
				}
			}
		}
		else if (m_Description.empty() == false)
		{
			append_format(Help_out, "\xee\x81\xaf %s : %s", m_Name.c_str(), m_Description.c_str());
		}
		else
		{
			append_format(Help_out, "No help available for command '%s'", m_Name.c_str());
		}

		return Help_out;
	}

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

	int WindowerCommand::Execute(std::string &Feedback_out)
	{
		if (m_pHandler != NULL)
			return m_pHandler->DefaultHandler(this, Feedback_out);
		else
			format(Feedback_out, "The command '%s' is not associated with any action.", m_Name.c_str());

		return DISPATCHER_RESULT_INVALID_CALL;
	}

	const std::string WindowerCommand::GetStringValue(const std::string &Name_in) const
	{
		CommandParameter *pParam = GetParameter(Name_in);

		if (pParam != NULL)
			return pParam->GetStringValue();

		return "";
	}

	const string_t WindowerCommand::GetWideStringValue(const std::string &Name_in) const
	{
		CommandParameter *pParam = GetParameter(Name_in);
		string_t Result;

		if (pParam != NULL)
			convert_utf8(pParam->GetStringValue(), Result);

		return Result;
	}

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

	long WindowerCommand::GetPointerValue(const std::string &Name_in) const
	{
		CommandParameter *pParam = GetParameter(Name_in);

		if (pParam != NULL)
			return pParam->GetPointerValue();

		return 0L;
	}

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

	long WindowerCommand::GetIntegerValue(const std::string &Name_in) const
	{
		CommandParameter *pParam = GetParameter(Name_in);

		if (pParam != NULL)
			return pParam->GetIntegerValue();

		return 0L;
	}

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

	double WindowerCommand::GetFloatValue(const std::string &Name_in) const
	{
		CommandParameter *pParam = GetParameter(Name_in);

		if (pParam != NULL)
			return pParam->GetFloatValue();

		return 0.;
	}

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

	void WindowerCommand::Clear()	
	{
		CommandParams::const_iterator ParamIt, EndIt = m_Parameters.cend();

		for (ParamIt = m_Parameters.cbegin(); ParamIt != EndIt; ++ParamIt)
			delete ParamIt->second;

		Invalidate();
	}

	CommandParameter* WindowerCommand::GetParameter(const std::string &Name_in) const
	{
		CommandParams::const_iterator ParamIt = m_Parameters.find(Name_in);

		if (ParamIt != m_Parameters.cend())
			return ParamIt->second;

		return NULL;
	}

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

	bool WindowerCommand::OnUnregister()
	{ --m_RefCount; return (m_RefCount == 0); }
	void WindowerCommand::OnRegister()
	{ ++m_RefCount; }

	/*! \brief 
	*/
	size_t WindowerCommand::Count() const
	{
		return m_Parameters.size();
	}

	/*! \brief 
	*/
	bool WindowerCommand::IsEmpty() const
	{
		return m_Parameters.empty();
	}

	/*! \brief 
	*/
	CommandParams::const_iterator WindowerCommand::Begin() const
	{
		return m_Parameters.cbegin();
	}

	/*! \brief 
	*/
	CommandParams::const_iterator WindowerCommand::End() const
	{
		return m_Parameters.cend();
	}

	/*! \brief 
	*/
	bool WindowerCommand::IsPublic() const
	{
		return m_Public;
	}

	/*! \brief 
	\param[] Public_in : 
	*/
	void WindowerCommand::SetPublic(bool Public_in)
	{
		m_Public = Public_in;
	}

	/*! \brief 
	*/
	bool WindowerCommand::IsRestricted() const
	{
		return m_Restricted;
	}

	/*! \brief 
	\param[] Public_in : 
	*/
	void WindowerCommand::SetRestricted(bool Public_in)
	{
		m_Public = Public_in;
	}

	/*! \brief 
	*/
	const std::string& WindowerCommand::GetName() const
	{
		return m_Name;
	}

	/*! \brief 
	\param[] Name_in : 
	*/
	void WindowerCommand::SetName(const std::string &Name_in)
	{
		m_Name = Name_in;
	}

	/*! \brief 
	*/
	DWORD WindowerCommand::GetKey() const
	{
		return m_RegistrationKey;
	}

	/*! \brief 
	*/
	INT_PTR WindowerCommand::GetID() const
	{
		return m_ID;
	}

	/*! \brief 
	\param[] Key_in : 
	*/
	bool WindowerCommand::IsKeyMatching(DWORD Key_in) const
	{
		return (m_RegistrationKey == Key_in);
	}

	/*! \brief 
	*/
	bool WindowerCommand::ValidateParameters() const
	{
		return (m_MinParamsCount <= m_MaxParamsCount	\
			&& m_Parameters.size() >= m_MaxParamsCount);
	}

	/*! \brief 
	*/
	const CommandParams& WindowerCommand::GetParameters() const
	{
		return m_Parameters;
	}

	/*! \brief 
	*/
	size_t WindowerCommand::GetMinParams() const
	{
		return m_MinParamsCount;
	}

	/*! \brief 
	*/
	size_t WindowerCommand::GetMaxParams() const
	{
		return m_MaxParamsCount;
	}

	/*! \brief 
	*/
	size_t WindowerCommand::GetParametersCount() const
	{
		return m_Parameters.size();
	}
}