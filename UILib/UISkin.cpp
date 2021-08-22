#include "stdafx.h"
#include "UISkin.h"

CUISkin::CUISkin(LPCWSTR lpSkinName) : m_nRefCount(1), m_hFileMap(NULL), m_lpData(NULL)
{
	*m_szSkinPath = 0;
	LoadSkin(lpSkinName);
}

CUISkin::~CUISkin()
{
	if (m_hFileMap)
	{
		if (m_lpData)
			UnmapViewOfFile(m_lpData);

		CloseHandle(m_hFileMap);
	}
}

void CUISkin::AddRef()
{
	InterlockedIncrement(&m_nRefCount);
}

void CUISkin::Release()
{
	if (InterlockedDecrement(&m_nRefCount) == 0)
		delete this;
}

// lpSkinName 可以为路径、数据文件、数据内容、资源 Id
void CUISkin::LoadSkin(LPCWSTR lpSkinName)
{
	if ((UINT)lpSkinName >> 16 == 0)
	{
		HMODULE hModule = NULL;
		m_lpData = (LPSTR)LockResource(LoadResource(hModule, FindResourceW(hModule, lpSkinName, L"SKIN")));
		return;
	}

	if (*(int *)lpSkinName == 'JGK')
	{
		m_lpData = (LPSTR)lpSkinName;
		return;
	}

	wchar_t szFileName[MAX_PATH];
	int nPathLen;

	if (wcschr(lpSkinName, '\\'))	// 是完整路径
	{
		wcscpy_s(szFileName, lpSkinName);
		nPathLen = wcslen(szFileName);

		if (szFileName[nPathLen - 1] == '\\')
			nPathLen--;
	}
	else
	{
		GetModuleFileNameW(NULL, szFileName, MAX_PATH);
		LPWSTR lpFileName = wcsrchr(szFileName, '\\') + 1;

		nPathLen = lpFileName - szFileName;
		nPathLen += swprintf_s(lpFileName, MAX_PATH - nPathLen, L"Skin\\%s", lpSkinName);
	}

	if (nPathLen < 4 || _wcsicmp(szFileName + nPathLen - 4, L".skn"))
	{
		*(int *)(szFileName + nPathLen) = '\\';
		wcscpy_s(m_szSkinPath, szFileName);

#ifdef _DEBUG
		if (PathIsDirectoryW(szFileName))	// 优先使用皮肤目录，以方便调试
			return;
#endif

		wcscpy_s(szFileName + nPathLen, MAX_PATH - nPathLen, L".skn");
		nPathLen += 4;
	}

	class CSkinLock
	{
	public:
		CSkinLock(LPCWSTR lpName)
		{
			m_hMutex = CreateMutexW(NULL, FALSE, lpName);
			WaitForSingleObject(m_hMutex, INFINITE);
		}

		~CSkinLock()
		{
			ReleaseMutex(m_hMutex);
			CloseHandle(m_hMutex);
		}

	private:
		HANDLE m_hMutex;
	};

	// 使用 Skin 路径作为 Mutex、FileMap 的名字
	wchar_t szName[MAX_PATH];
	wcsncpy_s(szName, szFileName, nPathLen = min(nPathLen, MAX_PATH - 3));
	CharLowerBuffW(szName, nPathLen);

	for (int i = 0; i != nPathLen; i++)
	{
		if (szName[i] == '\\')
			szName[i] = '/';
	}

	wcscpy_s(szName + nPathLen, 3, L"/m");
	CSkinLock skinLock(szName);
	wcscpy_s(szName + nPathLen, 3, L"/s");

	m_hFileMap = OpenFileMappingW(FILE_MAP_READ, FALSE, szName);
	if (m_hFileMap)
	{
		m_lpData = (LPSTR)MapViewOfFile(m_hFileMap, FILE_MAP_READ, 0, 0, 0);
		return;
	}

	HANDLE hFile = CreateFileW(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	DWORD dwSize = GetFileSize(hFile, NULL);
	m_hFileMap = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, dwSize, szName);

	if (m_hFileMap)
	{
		m_lpData = (LPSTR)MapViewOfFile(m_hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
		ReadFile(hFile, m_lpData, dwSize, &dwSize, NULL);
	}

	CloseHandle(hFile);
}

// 二分法查找
static LPCSTR DoFind(LPCSTR lpBase, int *lpData, LPCWSTR lpFileName)
{
	for (int nBegin = 0, nEnd = *lpData++; nBegin != nEnd; )
	{
		int nIndex = (nBegin + nEnd) / 2;
		int nCmp = _wcsicmp((LPCWSTR)(lpBase + lpData[nIndex * 2]), lpFileName);

		if (nCmp == 0)
		{
			return lpBase + lpData[nIndex * 2 + 1];
		}
		else if (nCmp < 0)
		{
			nBegin = nIndex + 1;
		}
		else
			nEnd = nIndex;
	}

	return NULL;
}

LPCSTR CUISkin::FindFile(LPCSTR lpBase, LPWSTR lpFileName) const
{
	while (int *lpData = (int *)lpBase)
	{
		if (LPWSTR lpNameEnd = wcschr(lpFileName, '\\'))
		{
			// 查找目录
			*lpNameEnd = 0;
			lpBase = DoFind(lpBase, lpData, lpFileName);
			lpFileName = lpNameEnd + 1;
		}
		else
		{
			// 跳过目录，查找文件
			lpData += 1 + *lpData * 2;
			return DoFind(lpBase, lpData, lpFileName);
		}
	}

	return NULL;
}

CUIStream *CUISkin::GetStream(LPCWSTR lpFileName)
{
	if (lpFileName == NULL)
		return NULL;

	wchar_t szFileName[MAX_PATH];

	if (m_lpData == NULL)	// Debug 时，直接读本地文件
	{
		wcscpy_s(szFileName, m_szSkinPath);
		wcscat_s(szFileName, lpFileName);
		return CUIStream::FromFile(szFileName);
	}

	if (*(int *)m_lpData != 'JGK')
		return NULL;

	wcscpy_s(szFileName, lpFileName);

	LPCSTR lpData = FindFile(m_lpData + 4, szFileName);
	if (lpData == NULL)
		return NULL;

	return CUIStream::FromData(lpData + 4, *(int *)lpData);
}
