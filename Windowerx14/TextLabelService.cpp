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
	bool TextLabelService::Invoke(PluginFramework::ServiceParam &Params_in)
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
					UiTextLabel *pLabel = CreateLabel(m_pDevice, pParam->m_LabelName, pParam->m_X, pParam->m_Y, pParam->m_W,
													  pParam->m_H, pParam->m_FontName, 12, true, false, pParam->m_ARGBColor, m_pRenderer, true);
					// set the return value
					*pParam->m_ppUiLabel = pLabel;

					return (pLabel != NULL);
				}
			}
		}

		return false;
	}

	UiTextLabel* TextLabelService::CreateLabel(IDirect3DDevice9Wrapper *pDevice_in,
											   const string_t& Name_in, long X_in, long Y_in,
											   long W_in, long H_in, const string_t &FontName_in,
											   unsigned short FontSize_in, bool bBold_in,
											   bool bItalic_in, unsigned long ARGB_in,
											   TextLabelRenderer *pRenderer_in, bool Visibile_in)
	{
		UiTextLabel *pLabel = NULL;

		if (pRenderer_in != NULL && pDevice_in != NULL)
		{
			UiIdMap::const_iterator IdIt = m_LabelIDs.cbegin();
			UiLabelMap::const_iterator LabelIt;
			unsigned long LabelID = 0UL;

			if (IdIt != m_LabelIDs.cend())
				LabelID = IdIt->second;
			else
				LabelID = m_NextID;

			LabelIt = m_RemovedLabels.find(LabelID);

			if (LabelIt == m_RemovedLabels.cend())
			{
				pLabel = new UiTextLabel(m_NextID, pDevice_in, Name_in, X_in, Y_in, W_in, H_in, FontName_in,
										 FontSize_in, bBold_in, bItalic_in, ARGB_in, pRenderer_in, Visibile_in);

				// add it to the map
				m_LabelIDs[Name_in] = m_NextID;
				m_UiElements[m_NextID] = pLabel;				
				m_pDevice->AddRenderable(m_NextID++, pLabel);
			}
			else
			{
				pLabel = LabelIt->second;

				if (pLabel != NULL)
				{
					pLabel->SetName(Name_in);
					pLabel->SetPos(X_in, Y_in);
					pLabel->SetSize(W_in, H_in);
					pLabel->SetTitleColor(ARGB_in);
					pLabel->SetVisible(Visibile_in);				
					pLabel->SetTitleFont(FontName_in, FontSize_in, bBold_in, bItalic_in);
					pLabel->GetFont()->Initialize(m_pDevice, FontName_in.c_str(), FontSize_in, true, bBold_in, bItalic_in);
					// add it to the map
					m_UiElements[LabelID] = pLabel;
					m_pDevice->AddRenderable(LabelID, pLabel);
					m_RemovedLabels.erase(LabelIt);
				}
			}
		}

		return pLabel;
	}

	bool TextLabelService::DestroyLabel(unsigned long ID_in, IDirect3DDevice9Wrapper *pDevice_in)
	{
		if (pDevice_in != NULL)
		{
			RenderableMap::const_iterator LabelIt = m_UiElements.find(ID_in);
			IRenderable *pRenderable = NULL;
			bool Result = false;

			if (LabelIt != m_UiElements.cend())
			{
				pRenderable = LabelIt->second;

				if (LabelIt->second != NULL)
				{
					pRenderable->SetVisible(false);
					m_RemovedLabels[ID_in] = static_cast<UiTextLabel*>(pRenderable);
				}
				
				m_UiElements.erase(LabelIt);			
			}

			return (pDevice_in->RemoveRenderable(ID_in) && Result);
		}

		return false;
	}
}