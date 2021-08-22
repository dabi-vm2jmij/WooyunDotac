#include "stdafx.h"
#include "UIStream.h"
#include "UISkin.h"

namespace
{

class StreamMgr
{
public:
	StreamMgr()
	{
		InitializeSListHead(&m_listHead);

		for (int i = 0; i != sizeof(m_szBuffer) / sizeof(CUIStream); i++)
			InterlockedPushEntrySList(&m_listHead, (PSLIST_ENTRY)((CUIStream *)m_szBuffer + i));
	}

	void *Alloc()
	{
		return InterlockedPopEntrySList(&m_listHead);
	}

	bool Free(void *pData)
	{
		if (pData >= m_szBuffer && pData < m_szBuffer + sizeof(m_szBuffer))
		{
			InterlockedPushEntrySList(&m_listHead, (PSLIST_ENTRY)pData);
			return true;
		}

		return false;
	}

private:
	SLIST_HEADER m_listHead;
	char m_szBuffer[sizeof(CUIStream) * 16];
};

StreamMgr g_streamMgr;

}	// namespace

void *CUIStream::operator new(size_t nSize)
{
	void *pData;
	if (nSize > sizeof(CUIStream) || (pData = g_streamMgr.Alloc()) == NULL)
		pData = malloc(nSize);

	return pData;
}

void CUIStream::operator delete(void *pData)
{
	if (!g_streamMgr.Free(pData))
		free(pData);
}

// for Release
CUIStream *CUIStream::FromData(LPCVOID lpData, DWORD dwSize, bool bNeedFree)
{
	if (lpData == NULL || dwSize == 0)
		return NULL;

	CUIStream *pStream = new CUIStream(lpData, dwSize);

	if (bNeedFree)
	{
		if (pStream)
			pStream->m_dwCapacity = dwSize;		// 析构时 free
		else
			free((LPVOID)lpData);
	}

	return pStream;
}

// for Debug
CUIStream *CUIStream::FromFile(LPCWSTR lpFileName)
{
	HANDLE hFile = CreateFileW(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return NULL;

	DWORD dwSize = GetFileSize(hFile, NULL);
	LPSTR lpData = (LPSTR)malloc(dwSize);
	ReadFile(hFile, lpData, dwSize, &dwSize, NULL);
	CloseHandle(hFile);

	return FromData(lpData, dwSize, true);
}

CUIStream::CUIStream(LPCVOID lpData, DWORD dwSize) : m_nRefCount(1), m_dwCurPos(0), m_dwCapacity(0), m_dwSize(dwSize), m_lpData((LPSTR)lpData), m_nSkinId(0)
{
}

CUIStream::~CUIStream()
{
	if (m_lpData && m_dwCapacity)
		free(m_lpData);

	if (m_nSkinId)
		((CUISkin *)m_nSkinId)->Release();
}

HRESULT CUIStream::QueryInterface(REFIID riid, void **ppvObject)
{
	if (riid == IID_IUnknown || riid == IID_ISequentialStream || riid == IID_IStream)
	{
		AddRef();
		*ppvObject = this;
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG CUIStream::AddRef()
{
	return InterlockedIncrement(&m_nRefCount);
}

ULONG CUIStream::Release()
{
	ULONG nCount = InterlockedDecrement(&m_nRefCount);
	if (nCount == 0)
		delete this;

	return nCount;
}

HRESULT CUIStream::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
	DWORD dwSize = 0;

	if (m_dwSize > m_dwCurPos && (dwSize = m_dwSize - m_dwCurPos) > cb)
		dwSize = cb;

	if (dwSize)
	{
		CopyMemory(pv, m_lpData + m_dwCurPos, dwSize);
		m_dwCurPos += dwSize;
	}

	if (pcbRead)
		*pcbRead = dwSize;

	return S_OK;
}

HRESULT CUIStream::Write(const void *pv, ULONG cb, ULONG *pcbWritten)
{
	if (m_dwCapacity < m_dwCurPos + cb)
	{
		DWORD dwCapacity = m_dwCurPos + cb;

		if (dwCapacity < 256)
		{
			dwCapacity = 256;
		}
		else if (dwCapacity & (dwCapacity - 1))
		{
			// 指数增长
			DWORD dwIndex;
			_BitScanReverse(&dwIndex, dwCapacity);
			dwCapacity = 2 << dwIndex;
		}

		if (!Reserve(dwCapacity))
		{
			if (pcbWritten)
				*pcbWritten = 0;

			return E_OUTOFMEMORY;
		}
	}

	if (m_dwCurPos > m_dwSize)
		ZeroMemory(m_lpData + m_dwSize, m_dwCurPos - m_dwSize);

	if (cb)
	{
		CopyMemory(m_lpData + m_dwCurPos, pv, cb);
		m_dwCurPos += cb;
	}

	if (m_dwSize < m_dwCurPos)
		m_dwSize = m_dwCurPos;

	if (pcbWritten)
		*pcbWritten = cb;

	return S_OK;
}

HRESULT CUIStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
	switch (dwOrigin)
	{
	case STREAM_SEEK_SET:
		break;

	case STREAM_SEEK_CUR:
		dlibMove.LowPart += m_dwCurPos;
		break;

	case STREAM_SEEK_END:
		dlibMove.LowPart += m_dwSize;
		break;

	default:
		dlibMove.LowPart = -1;
	}

	HRESULT hr = S_OK;

	if ((long)dlibMove.LowPart < 0)
		hr = STG_E_SEEKERROR;
	else
		m_dwCurPos = dlibMove.LowPart;

	if (plibNewPosition)
		plibNewPosition->QuadPart = m_dwCurPos;

	return hr;
}

HRESULT CUIStream::SetSize(ULARGE_INTEGER libNewSize)
{
	if ((long)libNewSize.LowPart < 0 || !Reserve(libNewSize.LowPart))
		return E_OUTOFMEMORY;

	if (libNewSize.LowPart > m_dwSize)
		ZeroMemory(m_lpData + m_dwSize, libNewSize.LowPart - m_dwSize);

	m_dwSize = libNewSize.LowPart;
	return S_OK;
}

HRESULT CUIStream::CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
{
	ATLASSERT(0);
	return E_NOTIMPL;
}

HRESULT CUIStream::Commit(DWORD grfCommitFlags)
{
	ATLASSERT(0);
	return E_NOTIMPL;
}

HRESULT CUIStream::Revert()
{
	ATLASSERT(0);
	return E_NOTIMPL;
}

HRESULT CUIStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	ATLASSERT(0);
	return E_NOTIMPL;
}

HRESULT CUIStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	ATLASSERT(0);
	return E_NOTIMPL;
}

HRESULT CUIStream::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
	ZeroMemory(pstatstg, sizeof(STATSTG));
	pstatstg->type = STGTY_STREAM;
	pstatstg->cbSize.QuadPart = m_dwSize;
	return S_OK;
}

HRESULT CUIStream::Clone(IStream **ppstm)
{
	ATLASSERT(0);
	return E_NOTIMPL;
}

bool CUIStream::Reserve(DWORD dwCapacity)
{
	if (dwCapacity <= m_dwCapacity)
		return true;

	LPSTR lpData = NULL;

	if (m_dwCapacity == 0)
	{
		if (dwCapacity < m_dwSize)
			dwCapacity = m_dwSize;

		lpData = (LPSTR)malloc(dwCapacity);

		if (lpData && m_dwSize)
			CopyMemory(lpData, m_lpData, m_dwSize);
	}
	else
		lpData = (LPSTR)realloc(m_lpData, dwCapacity);

	if (lpData == NULL)
		return false;

	m_lpData = lpData;
	m_dwCapacity = dwCapacity;
	return true;
}
