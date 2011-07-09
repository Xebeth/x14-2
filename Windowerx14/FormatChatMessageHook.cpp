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

bool WINAPI FormatChatMessageHook(void* _this, USHORT MessageType, const GameChatTextObject* pSender, GameChatTextObject* pMessage)
{
	return g_pEngine->GameChat().FormatChatMessageHook(_this, MessageType, pSender, pMessage);
}