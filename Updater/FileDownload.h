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
	typedef std::basic_ofstream<unsigned char, std::char_traits<unsigned char> > FileStream;	

	class IThreadProgress;
	class StreamProgress;	
	class IThreadProc;

	class FileDownload : public IThreadProc
	{
	public:
		FileDownload(const string_t &URL_in, IThreadProgress *pProgress_in);
		virtual ~FileDownload();

		bool SaveToDisk(const TCHAR *pPath_in,
						const TCHAR *pDeflateFile_in = NULL) const;
		void SetURL(const string_t &URL_in);
		const string_t& GetURL() const;

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
