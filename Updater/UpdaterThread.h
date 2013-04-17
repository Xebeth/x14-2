/**************************************************************************
	created		:	2013-04-17
	filename	: 	UpdaterThread.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Updater thread
**************************************************************************/
#ifndef __I_UPDATER_THREAD_H__
#define __I_UPDATER_THREAD_H__

#ifndef _UPDATER_SDK_INCLUDE_
	#error Please include the global header file 'Updater.h'
#endif

namespace Updater
{
	class IThreadProc;

	class Thread
	{
	public:
		explicit Thread(IThreadProc *pThreadProc_in);
		virtual ~Thread();

		HANDLE Execute();
		void Abort();

	protected:
		IThreadProc *m_pThreadProc;
		HANDLE m_hThreadHandle;
		DWORD m_ThreadID;

	private:
		static DWORD WINAPI ThreadProc(LPVOID pThreadData_in);
	};
}

#endif//__IUPDATERTHREAD_H__
