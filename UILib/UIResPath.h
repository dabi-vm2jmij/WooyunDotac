#pragma once

// ���õ�ǰ��Դ�ļ������·��
// �÷���CUIResPath resPath(L"xxx")�������������Զ���ԭ�ϸ�����

class UILIB_API CUIResPath
{
public:
	CUIResPath(LPCWSTR lpCurPath = L"\\");
	~CUIResPath();

	void CombinePath(LPWSTR lpDstPath, LPCWSTR lpSrcPath, UINT *pnSubCnt = NULL) const;

private:
	CUIResPath *GetResPath() const;
	void SetResPath(CUIResPath *pResPath) const;

	CUIResPath *m_pNext;
	UINT        m_nSubCnt;		// ��·������
	UINT        m_nSubLens[8];	// ÿ����·������
	wchar_t     m_szCurPath[MAX_PATH];

	CUIResPath(const CUIResPath &) = delete;
	CUIResPath &operator=(const CUIResPath &) = delete;
};
