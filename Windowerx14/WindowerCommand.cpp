/**************************************************************************
	created		:	2011-06-07
	filename	: 	WindowerCommand.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <hash_map>
#include "WindowerCommand.h"

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
	WindowerCommand::WindowerCommand()
	{
		Invalidate();
	}

	/*! \brief Formats the help message for the command
		\param[out] Help_out : a string receiving the formatted help message
		\param[in] ShowValues_in : flag specifying if the description or value is displayed
		\return the formatted help message
	*/
	std::string& WindowerCommand::Output(std::string &Help_out, bool ShowValues_in)
	{
		if (Public && Parameters.empty() == false)
		{
			size_t ParamCount = 0, ParamSize = Parameters.size();
			CommandParameters::const_iterator Iter;
			std::string ParamDesc, ValueDesc;
			char *pParamFormat = NULL;

			Help_out.append(Name);

			if (Description.empty() == false)
				Help_out.append(" : " + Description);

			Help_out.append("\n");

			// if the descriptions aren't empty, their keys must match the parameter type ones
			for(Iter = Parameters.begin(); Iter != Parameters.end(); ++Iter)
			{
				if (ShowValues_in)
					ValueDesc = Iter->second.Value;
				else
					ValueDesc = Iter->second.Description;

				if (ValueDesc.empty() == false || ShowValues_in)
				{
					if (ParamCount < MinParamsCount - 1)
						pParamFormat = "\t[%s <%s>]\t: %s";
					else
						pParamFormat = "\t%s <%s>\t: %s";

					format(ParamDesc, pParamFormat, CommandParamTypes[Iter->second.Type], Iter->first.c_str(), ValueDesc.c_str());
				}
				else
				{
					if (ParamCount < MinParamsCount - 1)
						pParamFormat = "\t[%s <%s>]";
					else
						pParamFormat = "\t%s <%s>";

					format(ParamDesc, pParamFormat, CommandParamTypes[Iter->second.Type], Iter->first.c_str());
				}

				Help_out.append(ParamDesc.c_str());

				if (++ParamCount < ParamSize)
					Help_out.append("\n");
			}
		}
		else if (Description.empty() == false)
		{
			append_format(Help_out, "%s : %s", Name.c_str(), Description.c_str());
		}
		else
		{
			append_format(Help_out, "No help available for command '%s'", Name.c_str());
		}

		return Help_out;
	}

	//! \brief Sets a command to its default state
	void WindowerCommand::Invalidate()
	{
		MinParamsCount = MaxParamsCount = 0;
		CommandCallbackFunc = NULL;
		Name = Description = "";
		RegistrationKey = 0;
		Parameters.clear();
		Restricted = true;
		Public = false;		
	}
}