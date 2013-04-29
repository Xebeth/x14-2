/**************************************************************************
	created		:	2013-04-29
	filename	: 	Direct3DDevice9WrapperImpl.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __DIRECT3DDEVICE9_WRAPPER_IMPL_H__
#define __DIRECT3DDEVICE9_WRAPPER_IMPL_H__

class IRenderable;

class Direct3DDevice9WrapperImpl
{
	friend class IDirect3DDevice9Wrapper;
public:
	Direct3DDevice9WrapperImpl(const D3DPRESENT_PARAMETERS &PresentParams_in);

	bool AddRenderable(unsigned long ID_in, IRenderable *pRenderable_in);
	bool RemoveRenderable(unsigned long ID_in);

	void SetRendering(bool bEnable_in) { m_bRender = bEnable_in; }
	void ToggleRendering()	{ m_bRender = !m_bRender; }
	bool IsRendering() const { return m_bRender; }
	
private:
	void Draw();
	void OnDeviceLost(D3DPRESENT_PARAMETERS &PresentationParameters_in_out);
	void OnDeviceReset();
	D3DPRESENT_PARAMETERS m_PresentParams;
	RenderableMap m_UiElements;
	bool m_bSceneStarted;
	bool m_bFullscreen;
	bool m_bRender;
	bool m_bDrawUi;
};

#endif//__DIRECT3DDEVICE9_WRAPPER_IMPL_H__
