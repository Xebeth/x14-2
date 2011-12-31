/**************************************************************************
	created		:	2011-06-03
	filename	: 	FormatChatMessageHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Game chat hook definitions
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <HookEngine.h>

#include "WindowerSettings.h"
#include "WindowerEngine.h"

#include "FormatChatMessageHook.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "GameChatCore.h"

extern Windower::WindowerEngine *g_pEngine;

/*! \brief Formats a message received by the game chat log
	\param[in] pThis_in_out : a pointer to the class containing the hooked method
	\param[in] MessageType_in : the type of the message
	\param[in] pSender_in : the sender of the message
	\param[in,out] pMessage_in_out : the message
	\return true if the message was formatted successfully; false otherwise
*/
bool WINAPI FormatChatMessageHook(LPVOID pThis_in_out, USHORT MessageType_in, const StringNode* pSender_in, StringNode* pMessage_in_out)
{
	return g_pEngine->GameChat().FormatChatMessageHook(pThis_in_out, MessageType_in, pSender_in, pMessage_in_out);
}

/*! \brief Creates a text node from the specified string
	\param[out] pTextObject_out : a pointer to the class containing the hooked method
	\param[in] pText_in : the text used to initialize the node
	\param[in] TextLength_in : the length of the text
	\return a pointer to the text node
*/
StringNode* WINAPI CreateTextNodeHook(StringNode *pTextObject_out, const char *pText_in, UINT TextLength_in)
{
	return g_pEngine->GameChat().CreateTextNodeHook(pTextObject_out, pText_in, TextLength_in);
}