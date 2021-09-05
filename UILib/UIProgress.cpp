#include "stdafx.h"
#include "UIProgress.h"

CUIProgress::CUIProgress(CUIView *pParent, LPCWSTR lpFileName) : CUIControl(pParent), m_nMaxPos(100), m_nCurPos(0)
{
	m_bClickable = false;

	SplitImage(lpFileName, m_imagexs);
	SetSize(m_imagexs[0].Rect().Size());
}

CUIProgress::~CUIProgress()
{
}

void CUIProgress::MyPaint(CUIDC &dc) const
{
	CRect rect(m_rect);
	rect.right = rect.left + rect.Width() * m_nCurPos / m_nMaxPos;

	if (m_imagexs[1])
	{
		m_imagexs[0].StretchDraw(dc, m_rect);
		m_imagexs[1].StretchDraw(dc, rect);
	}
	else if (m_imagexs[0])
	{
		m_imagexs[0].StretchDraw(dc, rect);
	}
}

void CUIProgress::SetValue(int nCurPos, int nMaxPos)
{
	if (nCurPos < 0)
		nCurPos = 0;

	if (nMaxPos <= 0)
		nMaxPos = m_nMaxPos;

	if (nCurPos > nMaxPos)
		nCurPos = nMaxPos;

	if (m_nCurPos == nCurPos && m_nMaxPos == nMaxPos)
		return;

	m_nCurPos = nCurPos;
	m_nMaxPos = nMaxPos;
	InvalidateRect(NULL);
}

void CUIProgress::OnLoaded(const IUILoadAttrs &attrs)
{
	__super::OnLoaded(attrs);

	int nCurPos, nMaxPos;
	if (attrs.GetInt(L"curPos", &nCurPos) && attrs.GetInt(L"maxPos", &nMaxPos))
		SetValue(nCurPos, nMaxPos);
}
