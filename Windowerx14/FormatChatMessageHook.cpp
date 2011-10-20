/**************************************************************************
	created		:	2011-06-03
	filename	: 	FormatChatMessageHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Game chat hook definitions
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <NonCopyable.h>
#include <HookEngine.h>
#include <queue>

#include "BaseEngine.h"
#include "PluginEngine.h"
#include "WindowerEngine.h"

#include "FormatChatMessageHook.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "GameChatCore.h"

extern Windower::WindowerEngine *g_pEngine;

bool WINAPI FormatChatMessageHook(void* _this, USHORT MessageType, const StringObject* pSender, StringObject* pMessage)
{
	return g_pEngine->GameChat().FormatChatMessageHook(_this, MessageType, pSender, pMessage);
}

StringObject* WINAPI CreateXmlNodeHook(StringObject *pTextObject_out, const char *pText_in, UINT TextLength_in)
{
	return g_pEngine->GameChat().CreateXmlNodeHook(pTextObject_out, pText_in, TextLength_in);
}