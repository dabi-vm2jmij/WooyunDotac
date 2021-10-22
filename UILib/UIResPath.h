#pragma once

// 设置当前资源文件的相对路径
// 用法：CUIResPath resPath(L"xxx")，超出作用域自动还原上个设置

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
	UINT        m_nSubCnt;		// 子路径个数
	UINT        m_nSubLens[8];	// 每个子路径长度
	wchar_t     m_szCurPath[MAX_PATH];

	CUIResPath(const CUIResPath &) = delete;
	CUIResPath &operator=(const CUIResPath &) = delete;
};
