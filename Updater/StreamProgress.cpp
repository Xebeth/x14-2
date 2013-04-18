/**************************************************************************
	created		:	2013-04-17
	filename	: 	StreamProgress.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "IThreadProgress.h"
#include "StreamProgress.h"

namespace Updater
{
	StreamProgress::StreamProgress(IThreadProgress *pProgress_in)
		: m_bCancelling(false), m_pProgress(pProgress_in), m_bCompleted(false),
		  m_BufferPos(0UL), m_BufferSize(0UL), m_pBinding(NULL) {}

	StreamProgress::~StreamProgress()
	{
		m_pProgress = NULL;

		if (m_pBinding != NULL)
		{
			m_pBinding->Release();
			m_pBinding = NULL;
		}
	}

	HRESULT StreamProgress::OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
	{
		if (m_pProgress != NULL)
			m_pProgress->OnProgress(ulProgress, ulProgressMax, szStatusText);

		return m_bCancelling ? E_ABORT : S_OK;
	}

	HRESULT StreamProgress::GetBindInfo(DWORD *grfBINDF, BINDINFO *pbindinfo)
	{
		if (pbindinfo != NULL)
		{
			// initialize the binding info
			memset(pbindinfo, 0, sizeof(*pbindinfo));		
			pbindinfo->cbSize = sizeof(*pbindinfo);

			// set the options
			pbindinfo->dwOptions = BINDINFO_OPTIONS_ENABLE_UTF8;
			pbindinfo->dwCodePage = CP_UTF8;
		}

		if (grfBINDF != NULL)
		{
			*grfBINDF = BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE 
					  | BINDF_GETNEWESTVERSION | BINDF_PRAGMA_NO_CACHE
					  | BINDF_PREFERDEFAULTHANDLER | BINDF_NO_UI
					  | BINDF_SILENTOPERATION | BINDF_FROMURLMON;
		}

		return m_bCancelling ? E_ABORT : S_OK;
	}

	HRESULT StreamProgress::OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC *pformatetc, STGMEDIUM *pstgmed)
	{
		unsigned long DataToRead = 0UL, BufferAlloc = 0UL, CurrentAlloc = 0UL, DataRead = 0UL;
		HRESULT hResult = S_OK;

		if ((grfBSCF & BSCF_FIRSTDATANOTIFICATION) == BSCF_FIRSTDATANOTIFICATION)
		{
			Reset();
			// download starting
			if (m_pProgress != NULL)
				m_pProgress->OnStart();
		}

		do
		{
			CurrentAlloc = m_Buffer.size();
			// read in big gulps to spin as little as possible
			DataToRead = max(CurrentAlloc - m_BufferSize, READ_CHUNK_SIZE);
			// ensure our data buffer is large enough
			BufferAlloc = m_BufferSize + DataToRead;
			// grow the buffer if needed
			if (CurrentAlloc < BufferAlloc)
				m_Buffer.resize(BufferAlloc * 2);
			// read from the stream
			hResult = pstgmed->pstm->Read(&m_Buffer[m_BufferSize], DataToRead, &DataRead);
			m_BufferSize += DataRead;
			DataRead = 0UL;
		}
		while (hResult != E_PENDING && hResult != S_FALSE && SUCCEEDED(hResult));

		if ((grfBSCF & BSCF_LASTDATANOTIFICATION) == BSCF_LASTDATANOTIFICATION)
		{
			m_Buffer.resize(m_BufferSize);
			m_bCompleted = true;
			// download finished
			if (m_pProgress != NULL)
				m_pProgress->OnFinish();
		}

		return hResult;
	}

	void StreamProgress::Cancel()
	{
		if (m_pProgress != NULL)
			m_pProgress->OnCancel();

		if (m_pBinding != NULL)
			m_pBinding->Abort();

		m_bCancelling = true;
	}

	void StreamProgress::Reset()
	{
		m_bCompleted = m_bCancelling = false;
		m_BufferSize = m_BufferPos = 0UL;
		m_Buffer.resize(READ_CHUNK_SIZE);
		m_bCompleted = false;
		m_Buffer.clear();
	}

	void StreamProgress::PumpMessages()
	{
		if (m_pProgress == NULL)
		{
			MSG msg;

			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
			m_pProgress->PumpMessages();
	}

	const std::vector<char>& StreamProgress::GetBuffer(unsigned long &BufferSize_out) const
	{ BufferSize_out = m_BufferSize; return m_Buffer; }

	bool StreamProgress::IsCancelPending() const
	{ return m_bCancelling; }

	bool StreamProgress::IsCompleted() const
	{ return m_bCompleted; }

	// not implemented

	HRESULT StreamProgress::OnStartBinding(DWORD dwReserved, IBinding *pib)
	{
		m_pBinding = pib;

		if (m_pBinding != NULL)
			m_pBinding->AddRef();

		return m_bCancelling ? E_ABORT : S_OK;
	}

	HRESULT StreamProgress::OnStopBinding(HRESULT hresult, LPCWSTR szError)
	{
		if (m_pBinding != NULL)
		{
			m_pBinding->Release();
			m_pBinding = NULL;
		}

		if (hresult != S_FALSE && hresult != S_OK)
		{
			if (m_pProgress != NULL)
				m_pProgress->OnProgress(0UL, 0UL, _T("Download failed..."));

			Cancel();
		}

		return m_bCancelling ? E_ABORT : S_OK;
	}

	HRESULT StreamProgress::GetPriority(LONG *pnPriority)
	{ return m_bCancelling ? E_ABORT : E_NOTIMPL; }

	HRESULT StreamProgress::OnLowResource(DWORD reserved)
	{ return m_bCancelling ? E_ABORT : E_NOTIMPL; }

	HRESULT StreamProgress::OnObjectAvailable(REFIID riid, IUnknown *punk)
	{ return m_bCancelling ? E_ABORT : E_NOTIMPL; }

	HRESULT StreamProgress::QueryInterface(REFIID iid, void ** ppvObject)
	{ return m_bCancelling ? E_ABORT : E_NOTIMPL; }

	ULONG StreamProgress::Release(void)
	{ return 0UL; }

	ULONG StreamProgress::AddRef(void)
	{ return 0UL; }
}