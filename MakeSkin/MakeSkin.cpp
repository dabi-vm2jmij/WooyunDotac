// MakeSkin.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

DWORD g_dwSize;
LPSTR g_lpBase, g_lpData;

bool PackFolder(LPCWSTR lpPathName);
bool UnpackFile(LPCWSTR lpFileName);

int _tmain(int argc, TCHAR *argv[])
{
	if (argc < 2 || argc > 3)
	{
		MessageBoxW(NULL, L"命令行错误", NULL, MB_ICONERROR);
		return 1;
	}

	WIN32_FILE_ATTRIBUTE_DATA wfad;

	if (!GetFileAttributesExW(argv[1], GetFileExInfoStandard, &wfad))
	{
		MessageBoxW(NULL, argv[1], L"路径不存在", MB_ICONERROR);
		return 2;
	}

	if (wfad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		DWORD dwMem = 2;	// 内存 MB

		if (argc > 2)
		{
			LPWSTR lpStr = argv[2];
			dwMem = wcstoul(lpStr, &lpStr, 10);

			if (dwMem == 0 || dwMem > 1024 || *lpStr)
			{
				MessageBoxW(NULL, L"命令行错误", NULL, MB_ICONERROR);
				return 1;
			}
		}

		g_dwSize = dwMem << 20;

		if (!PackFolder(argv[1]))
			return 3;
	}
	else
	{
		if (argc > 2)
		{
			MessageBoxW(NULL, L"命令行错误", NULL, MB_ICONERROR);
			return 1;
		}

		if (!UnpackFile(argv[1]))
			return 4;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// 打包资源

bool CheckRange()
{
	if (g_lpData + 4 > g_lpBase + g_dwSize)
	{
		MessageBoxW(NULL, L"资源太多", NULL, MB_ICONERROR);
		return false;
	}

	// 4字节对齐
	*(int *)g_lpData = 0;
	g_lpData = (LPSTR)(((int)g_lpData + 3) & ~3);
	return true;
}

bool PushString(const std::wstring &str)
{
	int nSize = (str.size() + 1) * 2;
	LPSTR lpData = g_lpData;
	g_lpData += nSize;

	if (!CheckRange())
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
	LPSTR lpData = g_lpData;
	g_lpData += dwFileSize + 4;

	if (!CheckRange())
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

	LPSTR lpBase = g_lpData;
	g_lpData += (vecFolders.size() + vecFiles.size() + 1) * 8;

	if (!CheckRange())
		return false;

	int *lpData = (int *)lpBase;
	*lpData++ = vecFolders.size();

	for (auto it = vecFolders.begin(); it != vecFolders.end(); ++it)
	{
		*lpData++ = g_lpData - lpBase;

		if (!PushString(*it))
			return false;

		*lpData++ = g_lpData - lpBase;
		wcscpy_s(lpPathName + nPathLen, MAX_PATH - nPathLen, it->c_str());

		if (!PushFolder(lpPathName))	// 递归读取每个子目录
			return false;
	}

	*lpData++ = vecFiles.size();

	for (auto it = vecFiles.begin(); it != vecFiles.end(); ++it)
	{
		*lpData++ = g_lpData - lpBase;

		if (!PushString(*it))
			return false;

		*lpData++ = g_lpData - lpBase;
		wcscpy_s(lpPathName + nPathLen, MAX_PATH - nPathLen, it->c_str());

		if (!PushFile(lpPathName))
			return false;
	}

	return true;
}

bool PackFolder(LPCWSTR lpPathName)
{
	if ((g_lpBase = g_lpData = (LPSTR)malloc(g_dwSize)) == NULL)
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

	DWORD dwSize = g_lpData - g_lpBase, dwWritten;
	BOOL bRes1 = WriteFile(hFile, "KGJ", 4, &dwWritten, NULL) && dwWritten == 4;
	BOOL bRes2 = WriteFile(hFile, g_lpBase, dwSize, &dwWritten, NULL) && dwWritten == dwSize;
	free(g_lpBase);
	CloseHandle(hFile);

	if (!bRes1 || !bRes2)
	{
		MessageBoxW(NULL, szPathName, L"WriteFile", MB_ICONERROR);
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// 解包资源

int GetNewPathName(LPWSTR lpPathName)
{
	int nPathLen = 0;

	for (int i = 0; lpPathName[i]; i++)
	{
		if (lpPathName[i] == '\\' || lpPathName[i] == '/')
			nPathLen = i + 1;
	}

	SYSTEMTIME st;
	GetLocalTime(&st);

	return nPathLen + swprintf_s(lpPathName + nPathLen, MAX_PATH - nPathLen, L"%d%02d%02d_%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

bool PopToFile(LPCSTR lpData, LPCWSTR lpFileName)
{
	HANDLE hFile = CreateFileW(lpFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBoxW(NULL, lpFileName, L"CreateFile", MB_ICONERROR);
		return false;
	}

	DWORD dwFileSize = *(int *)lpData, dwWritten;
	bool bRet = WriteFile(hFile, lpData + 4, dwFileSize, &dwWritten, NULL) && dwWritten == dwFileSize;
	CloseHandle(hFile);

	if (!bRet)
		MessageBoxW(NULL, lpFileName, L"WriteFile", MB_ICONERROR);

	return bRet;
}

bool PopToFolder(LPCSTR lpBase, LPWSTR lpPathName)
{
	if (!CreateDirectoryW(lpPathName, NULL))
	{
		MessageBoxW(NULL, lpPathName, L"CreateDirectory", MB_ICONERROR);
		return false;
	}

	int nPathLen = wcslen(lpPathName);
	lpPathName[nPathLen++] = '\\';

	int *lpData = (int *)lpBase;

	for (int i = 0, n = *lpData++; i != n; i++, lpData += 2)
	{
		wcscpy_s(lpPathName + nPathLen, MAX_PATH - nPathLen, (LPCWSTR)(lpBase + *lpData));

		if (!PopToFolder(lpBase + lpData[1], lpPathName))
			return false;
	}

	for (int i = 0, n = *lpData++; i != n; i++, lpData += 2)
	{
		wcscpy_s(lpPathName + nPathLen, MAX_PATH - nPathLen, (LPCWSTR)(lpBase + *lpData));

		if (!PopToFile(lpBase + lpData[1], lpPathName))
			return false;
	}

	return true;
}

bool UnpackFile(LPCWSTR lpFileName)
{
	HANDLE hFile = CreateFileW(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBoxW(NULL, lpFileName, L"CreateFile", MB_ICONERROR);
		return false;
	}

	DWORD dwFileSize = GetFileSize(hFile, NULL);
	if (dwFileSize % 4 || dwFileSize < 12)
	{
		CloseHandle(hFile);
		MessageBoxW(NULL, lpFileName, L"验证错误", MB_ICONERROR);
		return false;
	}

	bool bRet = false;

	if (LPSTR lpData = (LPSTR)malloc(dwFileSize))
	{
		DWORD dwRead;

		if (ReadFile(hFile, lpData, dwFileSize, &dwRead, NULL) && dwRead == dwFileSize)
		{
			if (*(int *)lpData == 'JGK')
			{
				wchar_t szPathName[MAX_PATH];
				wcscpy_s(szPathName, lpFileName);
				int nPathLen = GetNewPathName(szPathName);

				if (bRet = PopToFolder(lpData + 4, szPathName))
				{
					szPathName[nPathLen] = 0;
					MessageBoxW(NULL, szPathName, L"解包成功", MB_ICONINFORMATION);
				}
			}
			else
				MessageBoxW(NULL, lpFileName, L"验证错误", MB_ICONERROR);
		}
		else
			MessageBoxW(NULL, lpFileName, L"ReadFile", MB_ICONERROR);

		free(lpData);
	}
	else
		MessageBoxW(NULL, L"内存不够", NULL, MB_ICONERROR);

	CloseHandle(hFile);
	return bRet;
}
