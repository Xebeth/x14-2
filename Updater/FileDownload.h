/**************************************************************************
	created		:	2013-04-17
	filename	: 	FileDownload.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Stream updater
**************************************************************************/
#ifndef __FILE_DOWNLOAD_H__
#define __FILE_DOWNLOAD_H__

#ifndef _UPDATER_SDK_INCLUDE_
	#error Please include the global header file 'Updater.h'
#endif

namespace Updater
{
	class IThreadProgress;
	class StreamProgress;	
	class IThreadProc;

	class FileDownload : public IThreadProc
	{
	public:
		FileDownload(const string_t &UpdateURL_in, IThreadProgress *pProgress_in);
		virtual ~FileDownload();

		const std::vector<char>& GetBuffer(unsigned long &BufferSize_out) const;
		bool SaveToDisk(const TCHAR *pFilename_in) const;		
		const string_t& GetUpdateURL() const;

		virtual bool IsCancelPending() const;
		virtual bool IsCompleted() const;
		virtual void Initialize();
		virtual void Cleanup();
		virtual void Update();
		virtual void Cancel();

	protected:
		StreamProgress *m_pProgress;
		string_t m_UpdateURL;

	private:
		IBindCtx* m_pBindCtx;
		IMoniker* m_pMoniker;
	};
}

#endif//__FILE_DOWNLOAD_H__
