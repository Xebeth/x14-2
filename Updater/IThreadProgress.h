/**************************************************************************
	created		:	2013-04-17
	filename	: 	IThreadProgress.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __I_THREAD_PROGRESS_H__
#define __I_THREAD_PROGRESS_H__

namespace Updater
{
	class IThreadProgress
	{
	public:
		IThreadProgress();
		virtual ~IThreadProgress() {}

		virtual void OnProgress(unsigned long Completed_in, unsigned long Total_in) =0;
		virtual void OnFinish() =0;
		virtual void OnCancel() =0;
		virtual void OnStart() =0;
	};
}

#endif//__ITHREADPROGRESS_H__
