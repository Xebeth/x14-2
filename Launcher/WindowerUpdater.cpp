/**************************************************************************
	created		:	2013-04-18
	filename	: 	WindowerUpdater.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "WindowerUpdater.h"

namespace Windower
{
	WindowerUpdater::WindowerUpdater(const TCHAR *pURL_in, Updater::IThreadProgress *pProgress_in)
		: m_hThread(nullptr)
	{
		m_pDownloader = new Updater::FileDownload(pURL_in, pProgress_in);
		m_pThread = new Updater::Thread(m_pDownloader);
	}

	WindowerUpdater::~WindowerUpdater()
	{
		if (m_pThread != nullptr)
		{
			delete m_pThread;
			m_pThread = nullptr;
		}

		if (m_pDownloader != nullptr)
		{
			delete m_pDownloader;
			m_pDownloader = nullptr;
		}
	}

	void WindowerUpdater::Start()
	{
		if (m_pThread != nullptr)
			m_hThread = m_pThread->Execute();
	}

	void WindowerUpdater::Cancel() const
	{
		if (m_pThread != nullptr)
		{
			m_pThread->Abort();
			m_pThread->Reset();
		}
	}

	void WindowerUpdater::SaveFile(const TCHAR* pFilename_in, const TCHAR *pDeflateFile_in) const
	{
		if (m_pDownloader != nullptr)
			m_pDownloader->SaveToDisk(pFilename_in, pDeflateFile_in);
	}
}