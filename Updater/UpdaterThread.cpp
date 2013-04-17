/**************************************************************************
	created		:	2013-04-17
	filename	: 	UpdaterThread.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Updater thread
**************************************************************************/
#include "stdafx.h"

#include "UpdaterThread.h"
#include "IThreadProc.h"

namespace Updater
{
	Thread::Thread(IThreadProc *pThreadProc_in)
		: m_pThreadProc(pThreadProc_in), m_hThreadHandle(NULL), m_ThreadID(0UL) {}

	Thread::~Thread()
	{
		if (m_pThreadProc != NULL)
			m_pThreadProc->Cancel();

		WaitForSingleObject(m_hThreadHandle, INFINITE);
		m_pThreadProc = NULL;
	}

	HANDLE Thread::Execute()
	{
		if (m_pThreadProc != NULL)
			m_hThreadHandle = CreateThread(NULL, 0, &Thread::ThreadProc, m_pThreadProc, 0, &m_ThreadID);

		return m_hThreadHandle;	
	}

	DWORD WINAPI Thread::ThreadProc(LPVOID pThreadData_in)
	{
		if (pThreadData_in != NULL)
		{
			IThreadProc *pThreadProc = reinterpret_cast<IThreadProc*>(pThreadData_in);

			pThreadProc->Initialize();

			do
			{
				if (pThreadProc->IsCancelPending())
					return -1;

				pThreadProc->Update();
				Sleep(0);
			}
			while (pThreadProc->IsCompleted() == false);

			pThreadProc->Cleanup();
		}

		return 0;
	}

	void Thread::Abort()
	{
		if (m_pThreadProc != NULL)
			m_pThreadProc->Cancel();
	}
}