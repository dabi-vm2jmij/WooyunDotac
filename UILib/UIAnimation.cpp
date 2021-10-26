#include "stdafx.h"
#include "UIAnimation.h"

CUIAnimation::CUIAnimation(CUIView *pParent, LPCWSTR lpFileName) : CUIControl(pParent), m_nElapse(100), m_bLoop(false), m_nFrameIdx(0), m_uiTimer([this]{ OnUITimer(); })
{
	m_bClickable = false;

	m_imagex = GetImage(lpFileName);
	SetSize(m_imagex.Rect().Size());
}

void CUIAnimation::OnPaint(CUIDC &dc) const
{
	__super::OnPaint(dc);

	if (m_imagex)
		m_imagex.Draw(dc, m_rect);
}

void CUIAnimation::OnUITimer()
{
	if (++m_nFrameIdx == m_imagex.GetFrameCount())
	{
		m_nFrameIdx = 0;

		if (!m_bLoop)
		{
			Stop();
			return;
		}
	}

	m_imagex.SetFrameIndex(m_nFrameIdx);
	InvalidateRect();
}

void CUIAnimation::Start(bool bLoop)
{
	if (m_imagex.GetFrameCount() < 2)
	{
		ATLASSERT(0);
		return;
	}

	m_bLoop = bLoop;
	m_uiTimer.Start(m_nElapse);
}

void CUIAnimation::Stop()
{
	m_uiTimer.Stop();
}

void CUIAnimation::Reset()
{
	if (m_nFrameIdx)
	{
		m_nFrameIdx = -1;
		OnUITimer();
	}

	Stop();
}

void CUIAnimation::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	int nValue;
	if (attrs.GetInt(L"elapse", &nValue))
		SetElapse(nValue);

	if (attrs.GetInt(L"start"))
		Start();
}
