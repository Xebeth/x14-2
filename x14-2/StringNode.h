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
	typedef struct _StringNode
	{
		char		*pResBuf;				// +4
		DWORD_PTR	 dwCapacity;			// +8
		DWORD_PTR	 dwSize;				// +12
		DWORD_PTR	 dwUnknown2;			// +16
		bool		 bUnknown;				// +17
		bool		 UseInternal;			// +18
		char		 InternalBuffer[64];	// +19
	} StringNode;							// total size = 52 / with alignment = 54

	size_t UpdateNode(char *pText_in, size_t TextLen_in, StringNode &Node_in_out);
	void InitStringNode(StringNode &Node_in_out, const char *pMsg_in);
}
#endif//__STRINGNODE_H__
