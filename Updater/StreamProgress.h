/**************************************************************************
	created		:	2013-04-17
	filename	: 	StreamProgress.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __STREAM_PROGRESS_H__
#define __STREAM_PROGRESS_H__

#ifndef _UPDATER_SDK_INCLUDE_
	#error Please include the global header file 'Updater.h'
#endif

#define READ_CHUNK_SIZE 16384

namespace Updater
{
	class IThreadProgress;

	class StreamProgress : public IBindStatusCallback
	{
	public:
		StreamProgress(IThreadProgress *pProgress_in);
		virtual ~StreamProgress();

		// IBindStatusCallback
		virtual HRESULT __stdcall OnStartBinding(DWORD dwReserved, IBinding *pib);
		virtual HRESULT __stdcall GetPriority(LONG *pnPriority);
		virtual HRESULT __stdcall OnLowResource(DWORD reserved);

		virtual HRESULT __stdcall OnProgress(ULONG ulProgress, ULONG ulProgressMax,
											 ULONG ulStatusCode, LPCWSTR szStatusText);

		virtual HRESULT __stdcall OnStopBinding(HRESULT hresult, LPCWSTR szError);
		virtual HRESULT __stdcall GetBindInfo(DWORD *grfBINDF, BINDINFO *pbindinfo);

		virtual HRESULT __stdcall OnDataAvailable(DWORD grfBSCF, DWORD dwSize,
												  FORMATETC *pformatetc,
												  STGMEDIUM *pstgmed);

		virtual HRESULT __stdcall OnObjectAvailable(REFIID riid, IUnknown *punk);
		// IUnknown
		HRESULT __stdcall QueryInterface(REFIID iid, void ** ppvObject);
		ULONG __stdcall Release(void);
		ULONG __stdcall AddRef(void);

		const std::vector<char>& GetBuffer(unsigned long &BufferSize_out) const;
		bool IsCancelPending() const;
		bool IsCompleted() const;
		void PumpMessages();
		void Cancel();
		void Reset();
	protected:
		IThreadProgress *m_pProgress;
		std::vector<char> m_Buffer;
		unsigned long m_BufferSize;
		unsigned long m_BufferPos;
		IBinding *m_pBinding;
		bool m_bCancelling;
		bool m_bCompleted;
	};
}

#endif//__STREAM_PROGRESS_H__