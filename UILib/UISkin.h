#pragma once

// ������Դ��һ�� Debug ֱ�Ӷ�ÿ���ļ���Release ����ش����������ļ�

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
