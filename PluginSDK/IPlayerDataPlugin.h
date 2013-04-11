/**************************************************************************
	created		:	2013-04-06
	filename	: 	IPlayerDataPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __I_PLAYER_DATA_PLUGIN_H__
#define __I_PLAYER_DATA_PLUGIN_H__

#ifndef _WINDOWER_PLUGIN_SDK_INCLUDE_
	#error Please include the global header file 'PluginSDK.h'
#endif

namespace Windower
{
	typedef struct _TargetPos
	{
		float *pPosX, *pPosY, *pPosZ;
		const char *pTargetName;
	} TargetPos;

	//! \brief Base class for plugins subscribing to the FormatChatMessage service
	class IPlayerDataPlugin : public WindowerPlugin
	{
	public:
		/*! \brief IPlayerDataPlugin constructor
			\param[in] pServices_in : a pointer to the plugin services
		*/
		explicit IPlayerDataPlugin(PluginFramework::IPluginServices *pServices_in)
			: WindowerPlugin(pServices_in) {}

		/*! \brief Callback invoked when the game updates the player data pointer
			\param[in] pPlayerData_in : pointer to the player data
		*/
		virtual void OnPlayerPtrChange(const TargetPos &PlayerData_in) =0;
		/*! \brief Callback invoked when the game updates the player data pointer
			\param[in] pPlayerData_in : pointer to the player data
		*/
		virtual void OnTargetPtrChange(const TargetPos &TargetData_in) =0;

	protected:
		virtual bool Unsubscribe();
		virtual bool Subscribe();
	};
}

#endif//__I_PLAYER_DATA_PLUGIN_H__
