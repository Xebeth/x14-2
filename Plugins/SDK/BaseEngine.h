/**************************************************************************
	created		:	2011-07-03
	filename	: 	BaseEngine.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Interface for the engine
**************************************************************************/
#ifndef __BASE_ENGINE_H__
#define __BASE_ENGINE_H__

#ifndef _WINDOWER_PLUGIN_SDK_INCLUDE_
	#error Please include the global header file 'PluginSDK.h'
#endif

namespace Windower
{
	//! \brief Interface for the engine
	class BaseEngine : public NonCopyable
	{
	public:
		//! BaseEngine destructor
		virtual ~BaseEngine();

		/*! \brief Installs the internal hooks used by the windower
		\return true if the hooks were installed successfully; false otherwise
		*/
		virtual bool Attach() =0;
		/*! \brief Uninstalls the internal hooks used by the windower
		\return true if the hooks were uninstalled successfully; false otherwise
		*/
		virtual bool Detach() =0;
	};
}

#endif//__BASE_ENGINE_H__