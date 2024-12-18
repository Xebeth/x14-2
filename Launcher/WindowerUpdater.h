/**************************************************************************
	created		:	2013-04-18
	filename	: 	WindowerUpdater.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __WINDOWER_UPDATER_H__
#define __WINDOWER_UPDATER_H__

namespace Updater
{
	class IThreadProgress;
	class Thread;
}

namespace Windower
{
	class WindowerUpdater
	{
	public:
		WindowerUpdater(const TCHAR *pURL_in, Updater::IThreadProgress *pProgress_in);
		~WindowerUpdater();

		void SaveFile(const TCHAR* pFilename_in,
					  const TCHAR *pDeflateFile_in = nullptr) const;
		void Cancel() const;
		void Start();

	protected:
		Updater::FileDownload *m_pDownloader;
		Updater::Thread *m_pThread;
		HANDLE m_hThread;
	};
}

#endif//__WINDOWERUPDATER_H__
