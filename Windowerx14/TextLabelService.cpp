/**************************************************************************
	created		:	2013-04-13
	filename	: 	TextLabelService.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <d3dx9.h>
#include <d3d9.h>

#include "Font.h"
#include "IRenderable.h"
#include "UiTextLabel.h"

#include "IDirect3DDevice9Wrapper.h"

#include "ModuleService.h"
#include "TextLabelService.h"

#include "TextLabelRenderer.h"

namespace Windower
{
	TextLabelService::TextLabelService(const string_t& Name_in, RenderableMap &UiElements_in,
									   unsigned long BaseID_in, bool InvokePermission_in)
		: ModuleService(Name_in, InvokePermission_in), m_UiElements(UiElements_in),
		  m_NextID(BaseID_in), m_pDevice(NULL), m_pRenderer(NULL)
	{
		// add compatible plugins
		StringUtils::UUID PluginUUID;

		m_CompatiblePlugins.insert(PluginUUID.FromString(_T("F4F02060-9ED0-11E2-9E96-0800200C9A66")));	// Distance
	}

	/*! \brief Invokes the service
		\param[in] Params[in] : user data
		\return true if the service was invoked successfully; false otherwise
	*/
	bool TextLabelService::Invoke(const PluginFramework::ServiceParam &Params_in)
	{
		if (m_pDevice != NULL && Params_in.pData != NULL)
		{
			LabelCreationParam *pParam = reinterpret_cast<LabelCreationParam*>(Params_in.pData);
			NULL;

			if (pParam->pLabel != NULL)
			{
				if (pParam->Destroy)
				{
					UIAL::CUiWindow<> *pLabel = *pParam->pLabel;

					if (pLabel != NULL)
					{
						unsigned long ID = pLabel->GetID();
						RenderableMap::const_iterator LabelIt = m_UiElements.find(ID);

						if (LabelIt != m_UiElements.cend())
						{
							delete LabelIt->second;
							m_UiElements.erase(LabelIt);
						}

						m_pDevice->RemoveRenderable(ID);
						*pParam->pLabel = NULL;
					}

					return true;
				}
				else
				{
					// create a new label window
					UiTextLabel *pLabel = pLabel = new UiTextLabel(m_NextID, m_pDevice, pParam->pLabelName, pParam->X, pParam->Y, pParam->W,
																   pParam->H, _T("Arial"), 12, true, false, 0xFFFFFFFF, m_pRenderer, true);
					// add it to the map
					m_UiElements[m_NextID] = pLabel;
					// set the return value
					*pParam->pLabel = pLabel;

					return m_pDevice->AddRenderable(m_NextID++, pLabel);
				}
			}
		}

		return false;
	}

	void TextLabelService::SetRenderer(IDirect3DDevice9Wrapper *pDevice_in, TextLabelRenderer *pRenderer_in)
	{
		m_pRenderer = pRenderer_in;
		m_pDevice = pDevice_in;
	}
}