/**************************************************************************
	created		:	2013-04-11
	filename	: 	StringNode.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __STRINGNODE_H__
#define __STRINGNODE_H__

namespace Windower
{
	#pragma pack(push, 1)

	typedef struct _StringNode
	{
		char		*pResBuf;		// +4
		DWORD		 dwCapacity;	// +8
		DWORD		 dwSize;		// +12
		DWORD		 dwUnknown2;	// +16
		bool		 bUnknown;		// +17
		bool		 bUnknown2;		// +18
		char		 cBuf0;
		char		 cBuf1;
		char		 cBuf2;
		char		 cBuf3;
	} StringNode;

	#pragma pack(pop)

	size_t UpdateNode(char *pText_in, size_t TextLen_in, StringNode &Node_in_out);
}
#endif//__STRINGNODE_H__
