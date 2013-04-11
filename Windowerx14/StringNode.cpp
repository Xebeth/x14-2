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
			Node_in_out.cBuf0 = '\0';

		return Node_in_out.dwCapacity;
	}
}