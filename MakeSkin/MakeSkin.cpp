// MakeSkin.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Pack.h"
#include "Unpack.h"

int _tmain(int argc, TCHAR *argv[])
{
	if (argc < 2 || argc > 3)
	{
		MessageBox(NULL, _T("命令行错误"), NULL, MB_ICONERROR);
		return 1;
	}

	WIN32_FILE_ATTRIBUTE_DATA wfad;

	if (!GetFileAttributesEx(argv[1], GetFileExInfoStandard, &wfad))
	{
		MessageBox(NULL, argv[1], _T("路径不存在"), MB_ICONERROR);
		return 2;
	}

	if (wfad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		DWORD dwMem = 2;	// 内存 MB

		if (argc > 2)
		{
			LPTSTR lpStr = argv[2];
			dwMem = _tcstoul(lpStr, &lpStr, 10);

			if (dwMem == 0 || dwMem > 1024 || *lpStr)
			{
				MessageBox(NULL, _T("命令行错误"), NULL, MB_ICONERROR);
				return 1;
			}
		}

		if (!PackFolder(argv[1], dwMem << 20))
			return 3;
	}
	else
	{
		if (argc > 2)
		{
			MessageBox(NULL, _T("命令行错误"), NULL, MB_ICONERROR);
			return 1;
		}

		if (!UnpackFile(argv[1]))
			return 4;
	}

	return 0;
}
