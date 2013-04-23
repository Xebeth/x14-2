/**************************************************************************
	created		:	2013-04-11
	filename	: 	StringNode.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	String node structure
**************************************************************************/
#include "stdafx.h"

#include "StringNode.h"

namespace Windower
{
	size_t UpdateNode(char *pText_in, size_t TextLen_in, StringNode &Node_in_out)
	{
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
			Node_in_out.InternalBuffer[0] = '\0';

		return Node_in_out.dwCapacity;
	}

	void InitStringNode(StringNode &Node_in_out, const std::string &Msg_in)
	{
		size_t MsgLen = Msg_in.length() + 1;

		memset(&Node_in_out, 0, sizeof(Node_in_out));

		Node_in_out.dwSize = MsgLen;

		if (MsgLen > 0x40)
		{
			Node_in_out.dwCapacity = MsgLen;
			Node_in_out.bUnknown2 = false;
			// set the capacity to the next power of 2
			--Node_in_out.dwCapacity;
			Node_in_out.dwCapacity |= Node_in_out.dwCapacity >> 1;
			Node_in_out.dwCapacity |= Node_in_out.dwCapacity >> 2;
			Node_in_out.dwCapacity |= Node_in_out.dwCapacity >> 4;
			Node_in_out.dwCapacity |= Node_in_out.dwCapacity >> 8;
			Node_in_out.dwCapacity |= Node_in_out.dwCapacity >> 16;
			++Node_in_out.dwCapacity;
		}
		else
		{
			Node_in_out.bUnknown2 = true;
			Node_in_out.dwCapacity = 0x40;
			Node_in_out.pResBuf = (char*)&Node_in_out.InternalBuffer;
		}

		Node_in_out.pResBuf = (char*)Msg_in.c_str();
	}
}