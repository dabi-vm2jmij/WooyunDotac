#include "stdafx.h"
#include "UIProgress.h"

CUIProgress::CUIProgress(CUIView *pParent, LPCWSTR lpFileName) : CUIControl(pParent), m_nMaxPos(100), m_nCurPos(0)
{
	m_bClickable = false;

	m_imagex = ::GetImage(lpFileName);
	SetSize(m_imagex.Rect().Size());

	if (m_imagex.GetFrameCount() > 1)
	{
		SetBgImage(m_imagex);
		m_imagex.SetFrameIndex(1);
	}
}

void CUIProgress::OnPaint(CUIDC &dc) const
{
	__super::OnPaint(dc);

	int nWidth = m_rect.Width() * m_nCurPos / m_nMaxPos;
	if (nWidth && m_imagex)
	{
		CRect rect(m_rect);
		rect.right = rect.left + nWidth;

		if (m_imagex.Rect().Size() == m_rect.Size())
		{
			// 图片大小相同，不缩放
			CRect rcImg = m_imagex.Rect();
			rcImg.right = rcImg.left + nWidth;
			m_imagex.Draw(dc, rect, rcImg);
		}
		else
			m_imagex.Scale9Draw(dc, rect);
	}
}

void CUIProgress::SetMaxPos(int nMaxPos)
{
	if (nMaxPos <= 0 || m_nMaxPos == nMaxPos)
		return;

	if (m_nCurPos > nMaxPos)
		m_nCurPos = nMaxPos;

	m_nMaxPos = nMaxPos;
	InvalidateRect();
}

void CUIProgress::SetCurPos(int nCurPos)
{
	if (nCurPos < 0)
		nCurPos = 0;
	else if (nCurPos > m_nMaxPos)
		nCurPos = m_nMaxPos;

	if (m_nCurPos == nCurPos)
		return;

	m_nCurPos = nCurPos;
	InvalidateRect();
}

void CUIProgress::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	int nValue;
	if (attrs.GetInt(L"maxPos", &nValue))
		SetMaxPos(nValue);

	if (attrs.GetInt(L"curPos", &nValue))
		SetCurPos(nValue);
}
