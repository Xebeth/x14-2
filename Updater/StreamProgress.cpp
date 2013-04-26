/**************************************************************************
	created		:	2013-04-17
	filename	: 	StreamProgress.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <Realloc.h>

#include "IThreadProgress.h"
#include "StreamProgress.h"

namespace Updater
{
	StreamProgress::StreamProgress(IThreadProgress *pProgress_in)
		: m_bCancelling(false), m_pProgress(pProgress_in), m_bCompleted(false), m_pBuffer(NULL),
		  m_BufferPos(0UL), m_BufferSize(READ_CHUNK_SIZE), m_pBinding(NULL) {}

	StreamProgress::~StreamProgress()
	{
		m_pProgress = NULL;

		if (m_pBuffer != NULL)
		{
			free(m_pBuffer);
			m_pBuffer = NULL;
		}

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
		DWORD BufferAlloc = 0UL, DataToRead = 0UL, DataRead = 0UL;
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
			// read in big gulps to spin as little as possible
			if (DataRead < DataToRead)
				DataToRead -= DataRead;
			else
				DataToRead = max(m_BufferSize - m_BufferPos, READ_CHUNK_SIZE);
			// reset the amount of data read
			DataRead = 0UL;
			// ensure our data buffer is large enough
			BufferAlloc = m_BufferPos + DataToRead;
			// grow the buffer if needed
			if (m_BufferSize < BufferAlloc)
			{
				if (Buffer::Realloc(&m_pBuffer, BufferAlloc))
				{
					memset(m_pBuffer + m_BufferPos, 0, (BufferAlloc - m_BufferPos) * sizeof(BYTE));
					m_BufferSize = BufferAlloc;
				}
			}
			// read from the stream
			hResult = pstgmed->pstm->Read(m_pBuffer + m_BufferPos, DataToRead, &DataRead);
			m_BufferPos += DataRead;
		}
		while (hResult != E_PENDING && hResult != S_FALSE && SUCCEEDED(hResult));

		if ((grfBSCF & BSCF_LASTDATANOTIFICATION) == BSCF_LASTDATANOTIFICATION)
		{
			if (Buffer::Realloc(&m_pBuffer, m_BufferPos + 1))
				m_BufferSize = m_BufferPos + 1;

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
		m_bCompleted = false;

		if (m_pBuffer != NULL)
		{
			free(m_pBuffer);
			m_pBuffer = NULL;
		}
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

	BYTE* StreamProgress::GetBuffer(unsigned long &BufferSize_out) const
	{ BufferSize_out = m_BufferSize; return m_pBuffer; }

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