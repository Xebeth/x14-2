/**************************************************************************
	created		:	2011-11-06
	filename	: 	CommandParameter.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <hash_map>

#include "CommandParameter.h"

namespace Windower
{
	/*! \brief CommandParameter copy constructor
		\param[in] CommandParam_in : the command parameter to copy
	*/
	CommandParameter::CommandParameter(const CommandParameter &CommandParam_in)
	{
		m_Description = CommandParam_in.m_Description;
		m_Type = CommandParam_in.m_Type;
		m_Name = CommandParam_in.m_Name;		
	}
}