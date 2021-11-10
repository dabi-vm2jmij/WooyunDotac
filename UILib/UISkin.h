#pragma once

// 加载资源，一般 Debug 直接读每个文件，Release 则加载打包后的数据文件

class CUISkin
{
public:
	CUISkin(LPCWSTR lpSkinName);
	~CUISkin();

	UINT GetId() const { return m_nId; }
	IUIStream *GetStream(LPCWSTR lpFileName);

private:
	void LoadSkin(LPCWSTR lpSkinName);
	LPCSTR FindFile(LPCSTR lpBase, LPWSTR lpFileName) const;

	UINT    m_nId;
	LPVOID  m_lpAlloc;
	LPCVOID m_lpData;
	wchar_t m_szSkinPath[MAX_PATH];

	CUISkin(const CUISkin &) = delete;
	CUISkin &operator=(const CUISkin &) = delete;
};
