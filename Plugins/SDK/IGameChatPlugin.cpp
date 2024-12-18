/**************************************************************************
	created		:	2011-11-27
	filename	: 	IGameChatPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "ConfigurablePlugin.h"
#include "WindowerPlugin.h"
#include "IGameChatPlugin.h"

#define GAME_CHAT_MODULE			"GameChat"
// available services for the module
#define FORMAT_CHAT_MESSAGE_HOOK	"FormatChatMessage"

namespace Windower
{
	/*! \brief Adds the plugin as a subscriber to the game chat service
		\return true if the subscription succeeded; false otherwise
	*/
	bool IGameChatPlugin::Subscribe()
	{
		return SubscribeService(_T(GAME_CHAT_MODULE), _T(FORMAT_CHAT_MESSAGE_HOOK));
	}

	/*! \brief Removes the plugin as a subscriber to the game chat service
		\return true if the subscription was revoked successfully; false otherwise
	*/
	bool IGameChatPlugin::Unsubscribe()
	{
		return UnsubscribeService(_T(GAME_CHAT_MODULE), _T(FORMAT_CHAT_MESSAGE_HOOK));
	}

	bool IGameChatPlugin::ResizeBuffer(const char *pSrc_in, DWORD_PTR SrcSize_in, DWORD_PTR dwNewSize_in,
									   char **pBuffer_in_out, DWORD_PTR BufferSize_in, DWORD_PTR Offset_in) const
	{
		if (pBuffer_in_out != nullptr)
		{
			if (BufferSize_in < dwNewSize_in)
			{
				// allocate a new buffer
				char *pRealloc = (char*)realloc(*pBuffer_in_out, dwNewSize_in * sizeof(char));

				if  (pRealloc != nullptr)
				{
					// clear the buffer on the first allocation
					if (*pBuffer_in_out == nullptr)
					{
						// clear the buffer
						memset(pRealloc, 0, dwNewSize_in * sizeof(char));
						// copy the source
						memcpy_s(pRealloc + Offset_in,
								 sizeof(char) * (dwNewSize_in - Offset_in),
								 pSrc_in, SrcSize_in);
					}
					else
					{
						// clear the buffer
						memset(pRealloc + BufferSize_in, 0, (dwNewSize_in - BufferSize_in) * sizeof(char));

						if (Offset_in > 0UL)
						{
							// shift the buffer
							memcpy_s(pRealloc + Offset_in,
								sizeof(char) * (dwNewSize_in - Offset_in),
								*pBuffer_in_out, BufferSize_in);
						}
					}

					*pBuffer_in_out = pRealloc;
				}				
			}

			return true;
		}

		return false;
	}	
}
