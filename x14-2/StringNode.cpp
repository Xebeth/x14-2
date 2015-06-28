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
		DWORD_PTR newCapacity = Node_in_out.dwCapacity;

		Node_in_out.pResBuf = pText_in;
		Node_in_out.dwSize = Node_in_out.dwCapacity = TextLen_in;

		--newCapacity;
		newCapacity |= Node_in_out.dwCapacity >> 1;
		newCapacity |= Node_in_out.dwCapacity >> 2;
		newCapacity |= Node_in_out.dwCapacity >> 4;
		newCapacity |= Node_in_out.dwCapacity >> 8;
		newCapacity |= Node_in_out.dwCapacity >> 16;
		++newCapacity;

		if (newCapacity > Node_in_out.dwCapacity)
		{
			Node_in_out.dwCapacity = newCapacity;
			Node_in_out.UseInternal = (Node_in_out.dwCapacity <= 64);

			if (Node_in_out.UseInternal == false)
				Node_in_out.InternalBuffer[0] = '\0';
		}

		return Node_in_out.dwCapacity;
	}

	void InitStringNode(StringNode &Node_in_out, const char *pMsg_in)
	{
		memset(&Node_in_out, 0, sizeof(Node_in_out));

		if (pMsg_in != NULL)
		{
			size_t MsgLen = strlen(pMsg_in) + 1;

			Node_in_out.dwSize = MsgLen;

			if (MsgLen > 0x40)
			{
				Node_in_out.dwCapacity = MsgLen;
				Node_in_out.UseInternal = false;
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
				Node_in_out.UseInternal = true;
				Node_in_out.dwCapacity = 0x40;
				Node_in_out.pResBuf = (char*)&Node_in_out.InternalBuffer;
			}

			Node_in_out.pResBuf = (char*)pMsg_in;
		}
	}
}