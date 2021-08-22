#pragma once

// ������Դ��һ�� Debug ֱ�Ӷ�ÿ���ļ���Release ����ش����������ļ�

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
