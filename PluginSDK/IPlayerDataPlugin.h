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
#ifdef __USE_UIAL
	class LabelServiceParam
	{
	public:
		LabelServiceParam() : m_ppUiLabel(NULL), m_ARGBColor(0xFFFFFFFF),
			m_W(0UL), m_H(0UL), m_Destroy(false), m_X(0L), m_Y(0L) {}

		LabelServiceParam(UIAL::CUiWindow<> **ppUiLabel_in, const string_t &LabelName_in,
						  long X_in, long Y_in, unsigned long W_in, unsigned long H_in,
						  const string_t &FontName_in, unsigned long ARGBColor_in = 0xFFFFFFFF)
			: m_ppUiLabel(ppUiLabel_in), m_LabelName(LabelName_in), m_Destroy(false),
			  m_ARGBColor(ARGBColor_in), m_W(W_in), m_H(H_in), m_X(X_in), m_Y(Y_in) {}

		LabelServiceParam(UIAL::CUiWindow<> **ppUiLabel_in)
			: m_ppUiLabel(ppUiLabel_in), m_ARGBColor(0xFFFFFFFF),
			  m_W(0UL), m_H(0UL), m_Destroy(true), m_X(0L), m_Y(0L) {}

		UIAL::CUiWindow<> **m_ppUiLabel;
		unsigned long m_ARGBColor;
		unsigned long m_W, m_H;
		string_t m_LabelName;
		string_t m_FontName;		
		bool m_Destroy;
		long m_X, m_Y;
	};
#endif // __USE_UIAL

	typedef struct _TargetPos
	{
		_TargetPos() : pPosX(NULL), pPosY(NULL), pPosZ(NULL), pTargetName(NULL)
		{
#ifdef _DEBUG
			dwTargetAddr = 0UL;
#endif // _DEBUG
		}

		float *pPosX, *pPosY, *pPosZ;
		const char *pTargetName;
#ifdef _DEBUG
		DWORD dwTargetAddr;
#endif // _DEBUG
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
