/**************************************************************************
	created		:	2011-05-30
	filename	: 	PluginSupport.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin support structures
**************************************************************************/
#ifndef __PLUGIN_SUPPORT_H__
#define __PLUGIN_SUPPORT_H__

#ifndef PLUGIN_EXPORTS
	#define PLUGIN_API __declspec(dllimport)
#else
	#define PLUGIN_API __declspec(dllexport)
#endif

class PluginInfo
{
public:
	PluginInfo() : Handle(NULL) {}

	string_t ToString() const
	{
		return format(_T("Plugin '%s v%s' [0x%08X]:\n")
					  _T("\tFramework:\t%s\n")
					  _T("\tAuthor:\t\t%s\n")
					  _T("\tDescritpion:\t%s\n")
					  _T("\tPath:\t\t%s"),
					  Name.c_str(), PluginVersion.ToString().c_str(),
					  Handle, FrameworkVersion.ToString().c_str(),
					  Author.c_str(), Descritpion.c_str(),
					  DLLPath.c_str());
	}

	PluginFramework::PluginVersion	FrameworkVersion;
	PluginFramework::PluginVersion	PluginVersion;
	PluginFramework::PluginUUID		PluginIdentifier;
	string_t						Name;
	string_t						Author;
	string_t						Descritpion;
	string_t						DLLPath;
	HMODULE							Handle;
};

typedef PluginFramework::IPlugin* (*fnCreate)(); 
typedef void (*fnQuery)(PluginInfo&);
typedef void (*fnDestroy)(PluginFramework::IPlugin*);

typedef struct _RegisterParams
{
	PluginInfo	Info;
	fnQuery		QueryFunc;
	fnCreate	CreateFunc;
	fnDestroy	DestroyFunc;
} RegisterParams;

typedef bool (PluginFramework::IPluginServices::*fnInvokeService)(const string_t&, const string_t&,
																  const PluginFramework::ServiceParam&,
																  PluginFramework::ServiceParam&) const;
typedef bool (PluginFramework::IPluginServices::*fnSubscribeService)(const string_t&, const string_t&, PluginFramework::IPlugin*) const;
typedef bool (PluginFramework::IPluginServices::*fnUnsubscribeService)(const string_t&, const string_t&, PluginFramework::IPlugin*) const;

typedef void (*fnQuery)(PluginInfo&);
typedef RegisterParams* (*fnInitialize)(const PluginFramework::IPluginServices&);

extern "C"
{
	PLUGIN_API void QueryPlugin(PluginInfo&);
	PLUGIN_API RegisterParams* InitPlugin(const PluginFramework::IPluginServices&);
}

#endif//__PLUGIN_SUPPORT_H__