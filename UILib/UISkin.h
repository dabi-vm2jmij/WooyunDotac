#pragma once

// 加载资源，一般 Debug 直接读每个文件，Release 则加载打包后的数据文件

class CUISkin
{
public:
	CUISkin(LPCWSTR lpSkinName);
	~CUISkin();

	void AddRef();
	void Release();
	CUIStream *GetStream(LPCWSTR lpFileName);

private:
	void LoadSkin(LPCWSTR lpSkinName);
	LPCSTR FindFile(LPCSTR lpBase, LPWSTR lpFileName) const;

	ULONG   m_nRefCount;
	HANDLE  m_hFileMap;
	LPSTR   m_lpData;
	wchar_t m_szSkinPath[MAX_PATH];

	CUISkin(const CUISkin &) = delete;
	CUISkin &operator=(const CUISkin &) = delete;
};
