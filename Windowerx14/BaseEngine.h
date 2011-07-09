/**************************************************************************
	created		:	2011-07-03
	filename	: 	BaseEngine.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#ifndef __BASE_ENGINE_H__
#define __BASE_ENGINE_H__

class BaseEngine : public NonCopyable
{
public:
	virtual ~BaseEngine() {}

	virtual bool Attach() =0;
	virtual bool Detach() =0;
};

#endif//__BASE_ENGINE_H__