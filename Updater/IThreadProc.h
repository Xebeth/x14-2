/**************************************************************************
	created		:	2013-04-17
	filename	: 	IThreadProc.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __I_THREAD_PROC_H__
#define __I_THREAD_PROC_H__

#ifndef _UPDATER_SDK_INCLUDE_
	#error Please include the global header file 'Updater.h'
#endif

namespace Updater
{
	class IThreadProc
	{
	public:
		virtual ~IThreadProc() {}

		virtual bool IsCancelPending() const =0;
		virtual bool IsCompleted() const =0;
		virtual void Initialize() =0;
		virtual void Cleanup() =0;
		virtual void Update() =0;
		virtual void Cancel() =0;
	};
}

#endif//__I_THREAD_PROC_H__
