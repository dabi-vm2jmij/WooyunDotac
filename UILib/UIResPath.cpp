#include "stdafx.h"
#include "UIResPath.h"
#include "UILibApp.h"

CUIResPath::CUIResPath(LPCWSTR lpCurPath) : m_pNext(GetResPath())
{
	m_pNext->CombinePath(m_szCurPath, lpCurPath, &m_nSubCnt);
	SetResPath(this);

	for (LPWSTR lpPath = m_szCurPath + m_nSubLens[m_nSubCnt - 1]; *lpPath; )
	{
		if (LPWSTR lpFind = wcschr(lpPath, '\\'))
		{
			lpPath = lpFind + 1;
		}
		else
		{
			lpPath += wcslen(lpPath);
			*(int *)lpPath++ = '\\';
		}

		m_nSubLens[m_nSubCnt++] = lpPath - m_szCurPath;
	}
}

CUIResPath::~CUIResPath()
{
	auto pCurPath = GetResPath();
	if (pCurPath == this)
	{
		SetResPath(m_pNext);
		return;
	}

	for (CUIResPath *pPrePath; pPrePath = pCurPath; )
	{
		if ((pCurPath = pCurPath->m_pNext) == this)
		{
			pPrePath->m_pNext = m_pNext;
			break;
		}
	}
}

void CUIResPath::CombinePath(LPWSTR lpDstPath, LPCWSTR lpSrcPath, LPUINT pnSubCnt) const
{
	UINT nPreCnt = 0;	// 向上目录数

	if (lpSrcPath == NULL)
	{
		lpSrcPath = L"";
	}
	else if (*lpSrcPath == '\\')
	{
		lpSrcPath++;
		nPreCnt = MAXINT16;
	}
	else
	{
		// 计算 "..\" 个数
		for (; *(int *)lpSrcPath == '.\0.' && (lpSrcPath[2] == '\\' || lpSrcPath[2] == '/'); lpSrcPath += 3, nPreCnt++);
	}

	UINT nSubCnt = this ? m_nSubCnt : 0, nPathLen;

	if (nSubCnt > nPreCnt)
	{
		nSubCnt -= nPreCnt;
		nPathLen = m_nSubLens[nSubCnt - 1];
		CopyMemory(lpDstPath, m_szCurPath, nPathLen * 2);
	}
	else
	{
		nSubCnt  = 0;
		nPathLen = 0;
	}

	FormatPath(lpDstPath + nPathLen, lpSrcPath);

	if (pnSubCnt && (*pnSubCnt = nSubCnt))
		CopyMemory(pnSubCnt + 1, m_nSubLens, nSubCnt * 4);
}

CUIResPath *CUIResPath::GetResPath() const
{
	return g_theApp.GetResource().GetResPath();
}

void CUIResPath::SetResPath(CUIResPath *pResPath) const
{
	g_theApp.GetResource().SetResPath(pResPath);
}
