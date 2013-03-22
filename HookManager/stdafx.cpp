/**************************************************************************
	created		:	2011-05-09
	filename	: 	stdafx.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Precompiled headers support
**************************************************************************/
#include "stdafx.h"

#ifdef _DEBUG
	#ifdef _M_X64
		#pragma comment(lib, "Sigscan.x64d.lib")
	#else
		#pragma comment(lib, "Sigscan.x86d.lib")
	#endif
#else
	#ifdef _M_X64
		#pragma comment(lib, "Sigscan.x64.lib")
	#else
		#pragma comment(lib, "Sigscan.x86.lib")
	#endif
#endif