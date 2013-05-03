/**************************************************************************
	created		:	2011-05-09
	filename	: 	stdafx.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Precompiled headers support
**************************************************************************/
#include "stdafx.h"

#pragma comment(lib, "Comctl32.lib")

#ifdef _DEBUG
	#pragma comment(lib, "d3dx9d.lib")
	#pragma comment(lib, "d3d9.lib")
#else
	#pragma comment(lib, "d3dx9.lib")
	#pragma comment(lib, "d3d9.lib")
#endif // _DEBUG