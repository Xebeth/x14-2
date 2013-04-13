/**************************************************************************
	created		:	2013-04-11
	filename	: 	IServiceSubscriber.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Service subscriber interface
**************************************************************************/
#ifndef __I_SERVICE_SUBSCRIBER_H__
#define __I_SERVICE_SUBSCRIBER_H__

namespace Windower
{
	class IServiceSubscriber
	{
	public:
		IServiceSubscriber();
		virtual ~IServiceSubscriber();

		virtual void OnUnsubscribe(const string_t &ModuleName_in, const string_t &ServiceName_in) =0;
		virtual void OnSubscribe(const string_t &ModuleName_in, const string_t &ServiceName_in) =0;

	protected:

	};
}

#endif//__I_SERVICE_SUBSCRIBER_H__
