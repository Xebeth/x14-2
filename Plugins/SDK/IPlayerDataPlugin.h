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
	#pragma pack(push, 1)

	typedef struct _TargetData
	{						// Alignment	Offset
		DWORD dwUnknown00;	// 0030			+0
		DWORD dwUnknown01;	// 0034			+4
		DWORD dwUnknown02;	// 0038			+8
		DWORD dwUnknown03;	// 003C			+12
		DWORD dwUnknown04;	// 0040			+16
		DWORD dwUnknown05;	// 0044			+20
		DWORD dwUnknown06;	// 0048			+24
		DWORD dwUnknown07;	// 004C			+28
		DWORD dwUnknown08;	// 0050			+32
		DWORD dwUnknown09;	// 0054			+36
		DWORD dwUnknown10;	// 0058			+40
		DWORD dwUnknown11;	// 005C			+44
		char Name[112];		// 0060			+48
		float PosX;			// 00C0			+160
		float PosY;			// 00C4			+164
		float PosZ;			// 00C8			+168
	} TargetData;

	#pragma pack(pop)

#ifdef __USE_UIAL
	typedef void(__stdcall *fnOnMouseClick)(UIAL::CUiWindow<> *pUiElement_in_out, unsigned short X_in,
											unsigned short Y_in, DWORD_PTR MouseFlags_in,
											UINT_PTR KeyFlags_in);

	class LabelServiceParam
	{
	public:
		LabelServiceParam() : m_ppUiLabel(nullptr), m_TextColor(0xFFFFFFFFUL), m_FontName(_T("Arial")),
			m_W(0UL), m_H(0UL), m_Destroy(false), m_X(0L), m_Y(0L), m_Fontsize(12UL),
			m_Bold(true), m_Italic(false), m_pfnMouseClick(nullptr) {}

		LabelServiceParam(UIAL::CUiWindow<> **ppUiLabel_in, const string_t &LabelName_in,
						  long X_in, long Y_in, unsigned long W_in, unsigned long H_in,
						  const string_t &FontName_in = _T("Arial"), unsigned short FontSize_in = 12UL,
						  unsigned long TextColor_in = 0xFFFFFFFF, bool Bold_in = true, bool Italic_in = false)
			: m_ppUiLabel(ppUiLabel_in), m_LabelName(LabelName_in), m_Destroy(false), m_TextColor(TextColor_in),
			  m_W(W_in), m_H(H_in), m_X(X_in), m_Y(Y_in), m_Bold(Bold_in), m_Italic(Italic_in),
			  m_Fontsize(FontSize_in), m_FontName(FontName_in), m_pfnMouseClick(nullptr) {}

		LabelServiceParam(UIAL::CUiWindow<> **ppUiLabel_in)
			: m_ppUiLabel(ppUiLabel_in), m_TextColor(0xFFFFFFFF), m_FontName(_T("Arial")),
			  m_Fontsize(12UL), m_W(0UL), m_H(0UL), m_Destroy(true), m_X(0L), m_Y(0L),
			  m_Bold(true), m_Italic(false), m_pfnMouseClick(nullptr) {}

		
		UIAL::CUiWindow<> **m_ppUiLabel;
		fnOnMouseClick m_pfnMouseClick;
		unsigned long m_TextColor;
		unsigned short m_Fontsize;
		unsigned long m_W, m_H;
		string_t m_LabelName;
		string_t m_FontName;
		bool m_Collapsed;
		bool m_Destroy;
		bool m_Italic;
		bool m_Bold;		
		long m_X, m_Y;
	};
#endif // __USE_UIAL

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
			\param[in] PlayerData_in : player info
			\param[in] TargetData_in : target info
		*/
		virtual void OnTargetPtrChange(const TargetData **pPlayerData_in, const TargetData **pTargetData_in) = 0;

	protected:
		virtual bool Unsubscribe();
		virtual bool Subscribe();
	};
}

#endif//__I_PLAYER_DATA_PLUGIN_H__
