#include "stdafx.h"
#include "UILine.h"

CUILine::CUILine(CUIView *pParent) : CUIControl(pParent), m_color(-1), m_nStyle(0)
{
	m_bClickable = false;
}

// �����ߵ���ɫ��ʵ�߻�����
void CUILine::SetLineStyle(COLORREF color, int nStyle)
{
	m_color  = color;
	m_nStyle = (WORD)nStyle;
}

void CUILine::OnPaint(CUIDC &dc) const
{
	__super::OnPaint(dc);

	if (m_color == -1)
		return;

	int nSolid = (BYTE)m_nStyle, nSpace = m_nStyle >> 8;
	if (nSpace == 0)
		nSpace = nSolid;

	if (nSolid == 0 || m_rect.Width() == m_rect.Height())
	{
		dc.FillSolidRect(m_rect, m_color);
	}
	else if (m_rect.Width() > m_rect.Height())
	{
		// ����
		for (int nLeft = m_rect.left; nLeft < m_rect.right; nLeft += nSolid + nSpace)
		{
			dc.FillSolidRect(CRect(nLeft, m_rect.top, min(nLeft + nSolid, m_rect.right), m_rect.bottom), m_color);
		}
	}
	else
	{
		// ����
		for (int nTop = m_rect.top; nTop < m_rect.bottom; nTop += nSolid + nSpace)
		{
			dc.FillSolidRect(CRect(m_rect.left, nTop, m_rect.right, min(nTop + nSolid, m_rect.bottom)), m_color);
		}
	}
}

void CUILine::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	if (LPCWSTR lpStr = attrs.GetStr(L"color"))
	{
		COLORREF color = 0;
		ATLVERIFY(StrToColor(lpStr, color));
		SetLineStyle(color, attrs.GetInt(L"style", m_nStyle));
	}
}
