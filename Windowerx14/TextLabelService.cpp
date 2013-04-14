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

	void TextLabelService::SetRenderer(IDirect3DDevice9Wrapper *pDevice_in, TextLabelRenderer *pRenderer_in)
	{
		m_pRenderer = pRenderer_in;
		m_pDevice = pDevice_in;
	}

	/*! \brief Invokes the service
		\param[in] Params[in] : user data
		\return true if the service was invoked successfully; false otherwise
	*/
	bool TextLabelService::Invoke(const PluginFramework::ServiceParam &Params_in)
	{
		if (m_pDevice != NULL && Params_in.pData != NULL)
		{
			LabelServiceParam *pParam = reinterpret_cast<LabelServiceParam*>(Params_in.pData);
			NULL;

			if (pParam->m_ppUiLabel != NULL)
			{
				if (pParam->m_Destroy)
				{
					UIAL::CUiWindow<> *pLabel = *pParam->m_ppUiLabel;

					if (pLabel != NULL)
					{
						unsigned long ID = pLabel->GetID();
						
						*pParam->m_ppUiLabel = NULL;

						return DestroyLabel(ID, m_pDevice);
					}
				}
				else
				{
					// create a new label window
					UiTextLabel *pLabel = CreateLabel(m_NextID, m_pDevice, pParam->m_LabelName, pParam->m_X, pParam->m_Y, pParam->m_W,
													  pParam->m_H, _T("Arial"), 12, true, false, pParam->m_ARGBColor, m_pRenderer, true);
					// add it to the map
					m_UiElements[m_NextID] = pLabel;
					// set the return value
					*pParam->m_ppUiLabel = pLabel;

					return m_pDevice->AddRenderable(m_NextID++, pLabel);
				}
			}
		}

		return false;
	}

	UiTextLabel* TextLabelService::CreateLabel(unsigned long ID_in, IDirect3DDevice9Wrapper *pDevice_in,
											   const string_t& Name_in, long X_in, long Y_in,
											   long W_in, long H_in, const string_t &FontName_in,
											   unsigned short FontSize_in, bool bBold_in,
											   bool bItalic_in, unsigned long ARGB_in,
											   TextLabelRenderer *pRenderer_in, bool Visibile_in)
	{
		if (pRenderer_in != NULL && pDevice_in != NULL)
		{
			UiTextLabel *pLabel = new UiTextLabel(m_NextID, pDevice_in, Name_in, X_in, Y_in, W_in, H_in, _T("Arial"),
												  12, true, false, ARGB_in, pRenderer_in, Visibile_in);

			if (pLabel != NULL)
			{
				// add it to the map
				m_UiElements[m_NextID] = pLabel;
				// add it to the renderable elements
				pDevice_in->AddRenderable(m_NextID++, pLabel);
			}


			return pLabel;
		}

		return NULL;
	}

	bool TextLabelService::DestroyLabel(unsigned long ID_in, IDirect3DDevice9Wrapper *pDevice_in)
	{
		if (pDevice_in != NULL)
		{
			RenderableMap::const_iterator LabelIt = m_UiElements.find(ID_in);
			bool Result = false;

			if (LabelIt != m_UiElements.cend())
			{
				Result = true;
				delete LabelIt->second;
				m_UiElements.erase(LabelIt);			
			}

			return (pDevice_in->RemoveRenderable(ID_in) && Result);
		}

		return false;
	}
}