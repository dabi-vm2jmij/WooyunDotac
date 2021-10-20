#include "stdafx.h"
#include "UISkin.h"

CUISkin::CUISkin(LPCWSTR lpSkinName) : m_nRefCount(1), m_lpAlloc(NULL), m_lpData(NULL)
{
	*m_szSkinPath = 0;
	LoadSkin(lpSkinName);
}

CUISkin::~CUISkin()
{
	if (m_lpAlloc)
		free(m_lpAlloc);
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
		m_lpData = LockResource(LoadResource(hModule, FindResourceW(hModule, lpSkinName, L"SKIN")));
		return;
	}

	if (*(int *)lpSkinName == 'JGK')
	{
		m_lpData = lpSkinName;
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

	HANDLE hFile = CreateFileW(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	DWORD dwSize = GetFileSize(hFile, NULL);

	if (m_lpData = m_lpAlloc = malloc(dwSize))
		ReadFile(hFile, m_lpAlloc, dwSize, &dwSize, NULL);

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

	LPCSTR lpData = FindFile((LPCSTR)m_lpData + 4, szFileName);
	if (lpData == NULL)
		return NULL;

	return CUIStream::FromData((LPCSTR)lpData + 4, *(int *)lpData);
}
