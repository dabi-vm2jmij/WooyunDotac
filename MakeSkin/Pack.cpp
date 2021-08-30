#include "stdafx.h"
#include "Pack.h"

LPSTR g_lpBase;
DWORD g_dwSize, g_dwCapacity;

bool ReserveSize(DWORD dwSize)
{
	g_dwSize += (dwSize + 3) & ~3;	// 4字节对齐

	if (g_dwSize > g_dwCapacity)
	{
		MessageBoxW(NULL, L"资源太多", NULL, MB_ICONERROR);
		return false;
	}

	*(int *)(g_lpBase + g_dwSize - 4) = 0;
	return true;
}

bool PushString(const std::wstring &str)
{
	int nSize = (str.size() + 1) * 2;
	LPSTR lpData = g_lpBase + g_dwSize;

	if (!ReserveSize(nSize))
		return false;

	memcpy(lpData, str.c_str(), nSize);
	return true;
}

bool PushFile(LPCWSTR lpFileName)
{
	HANDLE hFile = CreateFileW(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBoxW(NULL, lpFileName, L"CreateFile", MB_ICONERROR);
		return false;
	}

	DWORD dwFileSize = GetFileSize(hFile, NULL);
	LPSTR lpData = g_lpBase + g_dwSize;

	if (!ReserveSize(dwFileSize + 4))
	{
		CloseHandle(hFile);
		return false;
	}

	*(int *)lpData = dwFileSize;

	DWORD dwRead;
	bool bRet = dwFileSize == 0 || ReadFile(hFile, lpData + 4, dwFileSize, &dwRead, NULL) && dwRead == dwFileSize;
	CloseHandle(hFile);

	if (!bRet)
		MessageBoxW(NULL, lpFileName, L"ReadFile", MB_ICONERROR);

	return bRet;
}

void InsertName(std::vector<std::wstring> &vecNames, LPCWSTR lpName)
{
	auto it = vecNames.begin();
	for (; it != vecNames.end() && _wcsicmp(it->c_str(), lpName) < 0; ++it);
	vecNames.insert(it, lpName);
}

bool PushFolder(LPWSTR lpPathName)
{
	int nPathLen = wcslen(lpPathName);
	lpPathName[nPathLen++] = '\\';
	*(int *)(lpPathName + nPathLen) = '*';

	std::vector<std::wstring> vecFolders;
	std::vector<std::wstring> vecFiles;

	WIN32_FIND_DATAW wfd;
	HANDLE hFind = FindFirstFileW(lpPathName, &wfd);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		MessageBoxW(NULL, lpPathName, L"FindFirstFile", MB_ICONERROR);
		return false;
	}

	do
	{
		if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			InsertName(vecFiles, wfd.cFileName);
		}
		else if (wfd.cFileName[0] != '.' || wfd.cFileName[1] && (wfd.cFileName[1] != '.' || wfd.cFileName[2]))
		{
			InsertName(vecFolders, wfd.cFileName);
		}
	} while (FindNextFileW(hFind, &wfd));

	FindClose(hFind);

	/* 格式
	+--------+-------------+-------------+---+--------+-------------+-------------+---+-------+---------+---+-------+---------+---+
	|目录个数|目录1名字偏移|目录1数据偏移|...|文件个数|文件1名字偏移|文件1数据偏移|...|目录名1|*格式数据|...|文件名1|大小+数据|...|
	+--------+-------------+-------------+---+--------+-------------+-------------+---+-------+---------+---+-------+---------+---+
	*/

	DWORD dwSize = g_dwSize;

	if (!ReserveSize((vecFolders.size() + vecFiles.size() + 1) * 8))
		return false;

	int *lpData = (int *)(g_lpBase + dwSize);
	*lpData++ = vecFolders.size();

	for (auto it = vecFolders.begin(); it != vecFolders.end(); ++it)
	{
		*lpData++ = g_dwSize - dwSize;

		if (!PushString(*it))
			return false;

		*lpData++ = g_dwSize - dwSize;
		wcscpy_s(lpPathName + nPathLen, MAX_PATH - nPathLen, it->c_str());

		if (!PushFolder(lpPathName))	// 递归读取每个子目录
			return false;
	}

	*lpData++ = vecFiles.size();

	for (auto it = vecFiles.begin(); it != vecFiles.end(); ++it)
	{
		*lpData++ = g_dwSize - dwSize;

		if (!PushString(*it))
			return false;

		*lpData++ = g_dwSize - dwSize;
		wcscpy_s(lpPathName + nPathLen, MAX_PATH - nPathLen, it->c_str());

		if (!PushFile(lpPathName))
			return false;
	}

	return true;
}

bool PackFolder(LPCWSTR lpPathName, DWORD dwCapacity)
{
	if ((g_lpBase = (LPSTR)malloc(g_dwCapacity = dwCapacity)) == NULL)
	{
		MessageBoxW(NULL, L"内存不够", NULL, MB_ICONERROR);
		return false;
	}

	wchar_t szPathName[MAX_PATH];
	wcscpy_s(szPathName, lpPathName);

	int nPathLen = wcslen(szPathName);
	if (szPathName[nPathLen - 1] == '\\' || szPathName[nPathLen - 1] == '/')
		szPathName[--nPathLen] = 0;

	if (!PushFolder(szPathName))
	{
		free(g_lpBase);
		return false;
	}

	// 写入同名文件
	wcscpy_s(szPathName + nPathLen, MAX_PATH - nPathLen, L".skn");

	HANDLE hFile = CreateFileW(szPathName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		free(g_lpBase);
		MessageBoxW(NULL, szPathName, L"CreateFile", MB_ICONERROR);
		return false;
	}

	DWORD dwWritten;
	bool bRes1 = WriteFile(hFile, "KGJ", 4, &dwWritten, NULL) && dwWritten == 4;
	bool bRes2 = WriteFile(hFile, g_lpBase, g_dwSize, &dwWritten, NULL) && dwWritten == g_dwSize;
	free(g_lpBase);
	CloseHandle(hFile);

	if (!bRes1 || !bRes2)
	{
		MessageBoxW(NULL, szPathName, L"WriteFile", MB_ICONERROR);
		return false;
	}

	return true;
}
