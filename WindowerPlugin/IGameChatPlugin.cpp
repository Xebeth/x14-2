/**************************************************************************
	created		:	2011-11-27
	filename	: 	IGameChatPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>

#include "IGameChatPlugin.h"

namespace Windower
{
	StringNode UpdateNode(char *pText_in, size_t TextLen_in, StringNode &Node_in_out)
	{
		StringNode OriginalNode(Node_in_out);

		Node_in_out.pResBuf = pText_in;
		Node_in_out.dwSize = Node_in_out.dwCapacity = TextLen_in;

		--Node_in_out.dwCapacity;
		Node_in_out.dwCapacity |= Node_in_out.dwCapacity >> 1;
		Node_in_out.dwCapacity |= Node_in_out.dwCapacity >> 2;
		Node_in_out.dwCapacity |= Node_in_out.dwCapacity >> 4;
		Node_in_out.dwCapacity |= Node_in_out.dwCapacity >> 8;
		Node_in_out.dwCapacity |= Node_in_out.dwCapacity >> 16;
		++Node_in_out.dwCapacity;

		Node_in_out.bUnknown2 = (Node_in_out.dwCapacity <= 64);

		if (Node_in_out.bUnknown2 == false)
			Node_in_out.cBuf0 = '\0';

		return OriginalNode;
	}

	/*! \brief Adds the plugin as a subscriber to the game chat service
		\return true if the subscription succeeded; false otherwise
	*/
	bool IGameChatPlugin::Subscribe()
	{
		return SubscribeService(_T("GameChat"), _T(FORMAT_CHAT_MESSAGE_HOOK));
	}

	/*! \brief Removes the plugin as a subscriber to the game chat service
		\return true if the subscription was revoked successfully; false otherwise
	*/
	bool IGameChatPlugin::Unsubscribe()
	{
		return UnsubscribeService(_T("GameChat"), _T(FORMAT_CHAT_MESSAGE_HOOK));
	}
}
