/**************************************************************************
	created		:	2013-04-17
	filename	: 	FileDownload.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Stream updater
**************************************************************************/
#include "stdafx.h"
#include <fstream>

#include "IThreadProc.h"
#include "FileDownload.h"

#include "StreamProgress.h"

namespace Updater
{
	Updater::FileDownload::FileDownload(const string_t &UpdateURL_in, IThreadProgress *pProgress_in)
		: m_UpdateURL(UpdateURL_in), m_pProgress(new StreamProgress(pProgress_in)),
		  m_pBindCtx(NULL), m_pMoniker(NULL) {}

	FileDownload::~FileDownload()
	{
		if (m_pProgress != NULL)
		{
			delete m_pProgress;
			m_pProgress = NULL;
		}
	}

	const string_t& FileDownload::GetUpdateURL() const
	{ return m_UpdateURL; }

	const std::vector<char>& FileDownload::GetBuffer(unsigned long &BufferSize_out) const
	{ return m_pProgress->GetBuffer(BufferSize_out); }

	bool FileDownload::SaveToDisk(const TCHAR *pFilename_in) const
	{
		if (pFilename_in != NULL && m_pProgress != NULL && m_pProgress->IsCompleted())
		{
			unsigned long BufferSize = 0UL, WritePosition = 0UL, DataToWrite = 0UL;
			const std::vector<char>&Buffer = m_pProgress->GetBuffer(BufferSize);
			std::ofstream File(pFilename_in, std::ios::out | std::ios::binary);

			while (WritePosition < BufferSize)
			{
				DataToWrite = max(BufferSize - WritePosition, READ_CHUNK_SIZE);
				File.write(&Buffer[WritePosition] , DataToWrite);

				WritePosition += DataToWrite;
			}
		}

		return false;
	}

	void FileDownload::Initialize()
	{
		HRESULT hResult = CreateAsyncBindCtx(NULL, m_pProgress, NULL, &m_pBindCtx);		

		if (SUCCEEDED(hResult) && m_pBindCtx != NULL)
		{
			hResult = CreateURLMoniker(NULL, m_UpdateURL.c_str(), &m_pMoniker);

			if (SUCCEEDED(hResult) && m_pMoniker != NULL)
			{
				hResult = RegisterBindStatusCallback(m_pBindCtx, m_pProgress, NULL, NULL);

				if (SUCCEEDED(hResult))
				{
					IStream *pStream = NULL;

					m_pMoniker->BindToStorage(m_pBindCtx, NULL, IID_IStream, (LPVOID*)&pStream);
				}
			}
		}

		if (m_pProgress != NULL && SUCCEEDED(hResult) == FALSE)
			m_pProgress->Cancel();
	}

	void FileDownload::Cleanup()
	{
		if (m_pMoniker != NULL)
		{
			m_pMoniker->Release();
			m_pMoniker = NULL;
		}

		if (m_pBindCtx != NULL)
		{
			m_pBindCtx->Release();
			m_pBindCtx = NULL;
		}
	}

	void FileDownload::Cancel()
	{
		if (m_pProgress != NULL)
			m_pProgress->Cancel();

		Cleanup();
	}

	bool FileDownload::IsCompleted() const
	{
		if (m_pProgress != NULL)
			return m_pProgress->IsCompleted();

		return true;
	}

	bool FileDownload::IsCancelPending() const
	{
		if (m_pProgress != NULL)
			return m_pProgress->IsCancelPending();

		return true;
	}

	void FileDownload::Update()
	{
		MSG msg;

		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}