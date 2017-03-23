/**************************************************************************
	created		:	2013-04-13
	filename	: 	TextLabelService.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>

#include "Font.h"
#include "IRenderable.h"
#include "UiTextLabel.h"

#include "IDirect3DDevice9Wrapper.h"
#include "Direct3DDevice9WrapperImpl.h"

#include "ModuleService.h"
#include "TextLabelService.h"

#include "TextLabelRenderer.h"

#include "WindowerEngine.h"

namespace Windower
{
	TextLabelService::TextLabelService(WindowerEngine &Engine_in, const string_t& Name_in,
									   RenderableMap &UiElements_in, unsigned long BaseID_in,
									   bool InvokePermission_in)
		: ModuleService(Name_in, InvokePermission_in), m_UiElements(UiElements_in), m_Engine(Engine_in),
		  m_NextID(BaseID_in), m_pDevice(nullptr), m_pRenderer(nullptr), m_pWrapperImpl(nullptr)
	{
		// add compatible plugins
		StringUtils::UUID PluginUUID;

		m_CompatiblePlugins.insert(PluginUUID.FromString(_T("F4F02060-9ED0-11E2-9E96-0800200C9A66")));	// Distance
		m_CompatiblePlugins.insert(PluginUUID.FromString(_T("61933EE0-3F0D-4EE4-B69E-C38B597293F0")));	// Gatherer
	}

	void TextLabelService::SetRenderer(TextLabelRenderer *pRenderer_in, IDirect3DDevice9 *pDevice_in,
									   Direct3DDevice9WrapperImpl *pWrapperImpl_in)
	{
		m_pWrapperImpl = pWrapperImpl_in;
		m_pRenderer = pRenderer_in;
		m_pDevice = pDevice_in;
	}

	/*! \brief Invokes the service
		\param[in] Params[in] : user data
		\return true if the service was invoked successfully; false otherwise
	*/
	bool TextLabelService::Invoke(PluginFramework::ServiceParam &Params_in)
	{
		if (m_pDevice != nullptr && m_pWrapperImpl != nullptr && Params_in.pData != nullptr)
		{
			LabelServiceParam *pParam = reinterpret_cast<LabelServiceParam*>(Params_in.pData);
			nullptr;

			if (pParam->m_ppUiLabel != nullptr)
			{
				if (pParam->m_Destroy)
				{
					UIAL::CUiWindow<> *pLabel = *pParam->m_ppUiLabel;

					if (pLabel != nullptr)
					{
						unsigned long ID = pLabel->GetID();
						
						*pParam->m_ppUiLabel = nullptr;

						return DestroyLabel(ID, m_pWrapperImpl);
					}
				}
				else
				{
					bool Deserialized = m_Engine.DeserializeLabel(pParam->m_LabelName, pParam->m_X, pParam->m_Y,
																  pParam->m_TextColor, pParam->m_FontName,
																  pParam->m_Fontsize, pParam->m_Bold,
																  pParam->m_Italic, pParam->m_Collapsed);
					// create a new label window
					UiTextLabel *pLabel = CreateLabel(m_pDevice, m_pWrapperImpl, pParam->m_LabelName,
													  pParam->m_X, pParam->m_Y, pParam->m_W,
													  pParam->m_H, pParam->m_FontName, 
													  pParam->m_Fontsize, pParam->m_Bold, pParam->m_Italic,
													  pParam->m_TextColor, m_pRenderer, true);

					pLabel->SetCollapsed(pParam->m_Collapsed);

					if (pParam->m_pfnMouseClick != nullptr)
						pLabel->SetMouseClickEvent(pParam->m_pfnMouseClick);

					// set the return value
					*pParam->m_ppUiLabel = pLabel;

					if (pLabel != nullptr && Deserialized == false)
						m_Engine.SerializeLabel(pParam->m_LabelName, pParam->m_X, pParam->m_Y,
												pParam->m_TextColor, pParam->m_FontName,
												pParam->m_Fontsize, pParam->m_Bold,
												pParam->m_Italic, pParam->m_Collapsed);

					return (pLabel != nullptr);
				}
			}
		}

		return false;
	}

	UiTextLabel* TextLabelService::CreateLabel(IDirect3DDevice9 *pDevice_in, Direct3DDevice9WrapperImpl *pWrapperImpl_in,
											   const string_t& Name_in, long X_in, long Y_in, long W_in, long H_in,
											   const string_t &FontName_in, unsigned short FontSize_in,
											   bool bBold_in, bool bItalic_in, unsigned long ARGB_in, 
											   TextLabelRenderer *pRenderer_in, bool Visibile_in)
	{
		UiTextLabel *pLabel = nullptr;

		if (pRenderer_in != nullptr && pDevice_in != nullptr)
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
				pWrapperImpl_in->AddRenderable(m_NextID++, pLabel);
			}
			else
			{
				pLabel = LabelIt->second;

				if (pLabel != nullptr)
				{
					pLabel->SetName(Name_in);
					pLabel->SetPos(X_in, Y_in);
					pLabel->SetSize(W_in, H_in);
					pLabel->SetTitleColor(ARGB_in);
					pLabel->SetVisible(Visibile_in);				
					pLabel->SetTitleFont(FontName_in, FontSize_in, bBold_in, bItalic_in);
					pLabel->GetFont()->Initialize(pDevice_in, FontName_in.c_str(),
												  FontSize_in, true, bBold_in, bItalic_in);
					// add it to the map
					m_UiElements[LabelID] = pLabel;
					pWrapperImpl_in->AddRenderable(LabelID, pLabel);
					m_RemovedLabels.erase(LabelIt);
				}
			}
		}

		return pLabel;
	}

	bool TextLabelService::DestroyLabel(unsigned long ID_in, Direct3DDevice9WrapperImpl *pWrapperImpl_in)
	{
		if (pWrapperImpl_in != nullptr)
		{
			RenderableMap::const_iterator LabelIt = m_UiElements.find(ID_in);
			IRenderable *pRenderable = nullptr;
			bool Result = false;

			if (LabelIt != m_UiElements.cend())
			{
				pRenderable = LabelIt->second;

				if (LabelIt->second != nullptr)
				{
					pRenderable->SetVisible(false);
					m_RemovedLabels[ID_in] = static_cast<UiTextLabel*>(pRenderable);
				}
				
				m_UiElements.erase(LabelIt);			
			}

			return (pWrapperImpl_in->RemoveRenderable(ID_in) && Result);
		}

		return false;
	}
}