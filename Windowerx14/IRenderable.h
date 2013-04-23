/**************************************************************************
	created		:	2013-04-12
	filename	: 	IRenderable.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __IRENDERABLE_H__
#define __IRENDERABLE_H__

struct IDirect3DDevice9;

class IRenderable
{
public:
	virtual ~IRenderable() {}

	virtual void SetVisible(bool Visible_in) =0;
	virtual bool IsVisible() const =0;
	virtual void OnDeviceReset() =0;
	virtual void OnDeviceLost() =0;
	virtual void Update() =0;	
	virtual bool Draw() =0;
};

typedef std::map<unsigned long, IRenderable*> RenderableMap;

#endif//__IRENDERABLE_H__
