/**************************************************************************
	created		:	2011-05-29
	filename	: 	VersionInfo.cpp
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin version
**************************************************************************/
#include "stdafx.h"

#include "VersionInfo.h"

namespace PluginFramework
{
	VersionInfo::VersionInfo() : Major(0), Minor(0), Build(0) {}

	VersionInfo::VersionInfo(short Major_in, short Minor_in, unsigned int Build_in)
		: Major(Major_in), Minor(Minor_in), Build(Build_in) {}

	VersionInfo::VersionInfo(const TCHAR *pVersion_in)
	{ FromString(pVersion_in); }

	bool VersionInfo::operator==(const VersionInfo& OtherVersion_in) const
	{
		return (OtherVersion_in.Major == Major
			 && OtherVersion_in.Minor == Minor
			 && OtherVersion_in.Build == Build);
	}

	bool VersionInfo::FromString(const TCHAR* pVersion_in) const
	{
		// ex : 1.0.25
		return (_stscanf_s(pVersion_in, _T("%d.%d.%d"),
						   &Major, &Minor, &Build) == 3);
	}

	string_t VersionInfo::ToString() const
	{
		return format(_T("%d.%d.%d"), Major, Minor, Build);
	}
}