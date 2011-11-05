/**************************************************************************
	created		:	2011-05-30
	filename	: 	PluginSupport.h
	author		:	Xebeth`
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

//! \brief Plugin information
class PluginInfo
{
public:
	//! PluginInfo default constructor
	PluginInfo() : Handle(NULL) {}

	/*! \brief Converts the plugin information to a string
		\return a string representation of the plugin information
	*/
	string_t ToString() const
	{
		return format(_T("Plugin '%s v%s':\n")
					  _T("\tAuthor:\t\t%s\n")
					  _T("\tDescritpion:\t%s\n"),
					  Name.c_str(), PluginVersion.ToString().c_str(),
					  Author.c_str(), Descritpion.c_str());
	}

	//! the compatible framework version
	PluginFramework::PluginVersion	FrameworkVersion;
	//! the plugin version
	PluginFramework::PluginVersion	PluginVersion;
	//! the plugin UUID
	PluginFramework::PluginUUID		PluginIdentifier;
	//! the name of the plugin
	string_t						Name;
	//! the author of the plugin
	string_t						Author;
	//! the description of the plugin
	string_t						Descritpion;
	//! the path of the DLL
	string_t						DLLPath;
	//! the URL used to check for updates
	string_t						UpdateUrl;
	//! the handle to the DLL
	HMODULE							Handle;
};

//! a function pointer to a Create function
typedef PluginFramework::IPlugin* (*fnCreate)(); 
//! a function pointer to a Query function
typedef void (*fnQuery)(PluginInfo&);
//! a function pointer to a Destroy function
typedef void (*fnDestroy)(PluginFramework::IPlugin*);

typedef struct _RegisterParams
{
	//! plugin
	PluginInfo	Info;
	//! plugin 'Query' function pointer
	fnQuery		QueryFunc;
	//! plugin 'Create' function pointer
	fnCreate	CreateFunc;
	//! plugin 'Destroy' function pointer
	fnDestroy	DestroyFunc;
} RegisterParams;

//! function pointer to an Initialize function
typedef RegisterParams* (*fnInitialize)(const PluginFramework::IPluginServices&);
//! function pointer to an InvokeService function
typedef bool (PluginFramework::IPluginServices::*fnInvokeService)(const string_t&, const string_t&,
																  const PluginFramework::ServiceParam&,
																  PluginFramework::ServiceParam&) const;
//! function pointer to a SubscribeService function
typedef bool (PluginFramework::IPluginServices::*fnSubscribeService)(const string_t&, const string_t&, PluginFramework::IPlugin*) const;
//! function pointer to an UnsubscribeService function
typedef bool (PluginFramework::IPluginServices::*fnUnsubscribeService)(const string_t&, const string_t&, PluginFramework::IPlugin*) const;

extern "C"
{
	/*! \brief Function exposed by the plugin DLL to retrieve the plugin information
		\param[in] PluginInfo_out : the plugin information
	*/
	PLUGIN_API void QueryPlugin(PluginInfo &PluginInfo_out);
	/*! \brief Function exposed by the plugin DLL to initialize the plugin object
		\param[in] PluginServices_in : the plugin services
		\return a pointer to the plugin registration parameters if successful; NULL otherwise
	*/
	PLUGIN_API RegisterParams* InitPlugin(const PluginFramework::IPluginServices &PluginServices_in);
}

#endif//__PLUGIN_SUPPORT_H__