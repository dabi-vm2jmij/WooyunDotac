#include "stdafx.h"
#include "UINotice.h"

CUINotice::CUINotice(CUIView *pParent) : CUIView(pParent), m_dir(ToBottom), m_nElapse(2000), m_bRunning(false), m_nCurPos(0), m_uiTimer([this]{ OnUITimer(); })
{
}

CUINotice::~CUINotice()
{
}

void CUINotice::RecalcLayout(LPRECT lpClipRect)
{
	if (m_rect.left == MAXINT16 || m_vecChilds.empty())
		return;

	int nIndex1 = m_nCurPos / 10;
	int nIndex2 = m_nCurPos % 10 ? (nIndex1 + 1) % m_vecChilds.size() : -1;

	for (int i = 0; i != m_vecChilds.size(); i++)
	{
		if (i != nIndex1 && i != nIndex2)
			FRIEND(m_vecChilds[i])->SetRect(NULL, lpClipRect);
	}

	CRect rect(m_rect);
	CSize size;

	switch (m_dir)
	{
	case ToLeft:
		rect.OffsetRect(-rect.Width() * (m_nCurPos % 10) / 10, 0);
		size.cx = rect.Width();
		break;

	case ToTop:
		rect.OffsetRect(0, -rect.Height() * (m_nCurPos % 10) / 10);
		size.cy = rect.Height();
		break;

	case ToRight:
		rect.OffsetRect(rect.Width() * (m_nCurPos % 10) / 10, 0);
		size.cx = -rect.Width();
		break;

	case ToBottom:
		rect.OffsetRect(0, rect.Height() * (m_nCurPos % 10) / 10);
		size.cy = -rect.Height();
		break;
	}

	FRIEND(m_vecChilds[nIndex1])->CalcRect(CRect(rect), lpClipRect);

	if (nIndex2 == -1)
		return;

	FRIEND(m_vecChilds[nIndex2])->CalcRect(rect + size, lpClipRect);
}

void CUINotice::OnUITimer()
{
	m_nCurPos = (m_nCurPos + 1) % (m_vecChilds.size() * 10);
	InvalidateLayout();

	if (m_nCurPos % 10)
		m_uiTimer.Start(30);
	else if (m_bRunning)
		m_uiTimer.Start(m_nElapse);
	else
		m_uiTimer.Stop();
}

void CUINotice::Start()
{
	if (m_vecChilds.size() < 2)
		return;

	m_bRunning = true;

	if (m_nCurPos % 10 == 0)
		m_uiTimer.Start(m_nElapse);
}

void CUINotice::Stop()
{
	m_bRunning = false;
}

void CUINotice::OnLoaded(const IUIXmlAttrs &attrs)
{
	__super::OnLoaded(attrs);

	int nValue;
	if (attrs.GetInt(L"elapse", &nValue))
		SetElapse(nValue);

	LPCWSTR lpStr = attrs.GetStr(L"dir");
	if (lpStr == NULL)
		return;

	struct
	{
		LPCWSTR lpStr;
		Dir dir;
	} items[] = { { L"toLeft", ToLeft }, { L"toTop", ToTop }, { L"toRight", ToRight }, { L"toBottom", ToBottom } };

	for (int i = 0; i != _countof(items); i++)
	{
		if (_wcsicmp(items[i].lpStr, lpStr) == 0)
		{
			SetDir(items[i].dir);
			return;
		}
	}

	ATLASSERT(0);
}
