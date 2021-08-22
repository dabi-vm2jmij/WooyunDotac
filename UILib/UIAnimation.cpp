#include "stdafx.h"
#include "UIAnimation.h"

CUIAnimation::CUIAnimation(CUIView *pParent, LPCWSTR lpFileName) : CUIControl(pParent), m_nElapse(100), m_bLoop(false), m_nFrameIdx(0), m_uiTimer([this]{ OnUITimer(); })
{
	m_bClickable = false;

	m_curImagex = GetImage(lpFileName);
	SetSize(m_curImagex.Rect().Size());
	ATLASSERT(m_curImagex.GetFrameCount() > 1);
}

CUIAnimation::~CUIAnimation()
{
}

void CUIAnimation::OnUITimer()
{
	if (++m_nFrameIdx == m_curImagex.GetFrameCount())
	{
		m_nFrameIdx = 0;

		if (!m_bLoop)
		{
			Stop();
			return;
		}
	}

	m_curImagex.SetFrameIndex(m_nFrameIdx);
	InvalidateRect(NULL);
}

void CUIAnimation::Start(bool bLoop)
{
	if (m_curImagex.GetFrameCount() < 2)
		return;

	m_bLoop = bLoop;
	m_uiTimer.Set(m_nElapse);
}

void CUIAnimation::Stop()
{
	m_uiTimer.Kill();
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

void CUIAnimation::OnLoaded(const IUILoadAttrs &attrs)
{
	__super::OnLoaded(attrs);

	int nValue;
	if (attrs.GetInt(L"elapse", &nValue))
		SetElapse(nValue);
}
