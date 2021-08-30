#include "stdafx.h"
#include "Unpack.h"

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
		MessageBoxW(NULL, lpFileName, L"验证失败", MB_ICONERROR);
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
				MessageBoxW(NULL, lpFileName, L"验证失败", MB_ICONERROR);
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
