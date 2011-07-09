/**************************************************************************
	created		:	2011-05-29
	filename	: 	PluginFramework.cpp
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Program entry point
**************************************************************************/
#include "stdafx.h"
#include <hash_map>
#include <vector>

#include "PluginUUID.h"
#include "PluginVersion.h"
#include "PluginSupport.h"
#include "IPlugin.h"
#include "IWindowerPlugin.h"
#include "PluginManager.h"

int _tmain(int argc, TCHAR* argv[])
{
	int Result = 0;
	if (argc == 1)
	{
		TCHAR *pExeName = _tcsrchr(argv[0], '\\');
		// _tprintf(_T("Usage:\t%s <plugins directory>\n"), ++pExeName);
	}
	else
	{
		string_t Version, PluginName = _T("Test plugin");
		PluginFramework::PluginManager Manager;

		_tprintf(_T("Plugin framework v%s\n"), Manager.GetVersionStr(Version).c_str());
		Result = Manager.ListPlugins(argv[1]);
		
		if (Manager.LoadPlugin(PluginName) == false)
		{
			_tprintf(_T("Failed to load pluging 'Test Plugin'...\n"));

			Result = -1;
		}
		else
		{
			Windower::IWindowerPlugin *pPlugin = Manager.CreateObject(PluginName);

			_tprintf(_T("%d\n"), pPlugin->Response());
		}
	}

	system("PAUSE");

	return Result;
}