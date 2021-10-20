#include "stdafx.h"
#include "UIGif.h"

CUIGif::CUIGif(CUIView *pParent, LPCWSTR lpFileName) : CUIControl(pParent), m_bLoop(false), m_nFrameIdx(0), m_uiTimer([this]{ OnUITimer(); })
{
	m_bClickable = false;

	IStream *pStream = GetStream(lpFileName);
	m_pImage = new Gdiplus::Image(pStream);
	pStream->Release();

	if (m_pImage->GetLastStatus() != Gdiplus::Ok)
		return;

	SetSize(CSize(m_pImage->GetWidth(), m_pImage->GetHeight()));

	UINT nCount = m_pImage->GetFrameDimensionsCount();
	GUID *pDimensionIDs = (GUID *)alloca(sizeof(GUID) * nCount + 1);

	if (nCount == 0 || m_pImage->GetFrameDimensionsList(pDimensionIDs, nCount) != Gdiplus::Ok || (nCount = m_pImage->GetFrameCount(&pDimensionIDs[0])) < 2)
		return;

	UINT nSize = m_pImage->GetPropertyItemSize(PropertyTagFrameDelay);
	Gdiplus::PropertyItem *pPropItem = (Gdiplus::PropertyItem *)alloca(nSize + 1);

	if (nSize == 0 || m_pImage->GetPropertyItem(PropertyTagFrameDelay, nSize, pPropItem) != Gdiplus::Ok)
		return;

	m_vecElapses.reserve(nCount);

	for (int i = 0; i != nCount; i++)
	{
		UINT nDelay = ((UINT *)pPropItem->value)[i] * 10;
		m_vecElapses.push_back(nDelay ? nDelay : 100);
	}
}

CUIGif::~CUIGif()
{
	delete m_pImage;
}

void CUIGif::DoPaint(CUIDC &dc) const
{
	Gdiplus::Graphics graphics(dc);
	graphics.DrawImage(m_pImage, m_rect.left, m_rect.top);
}

void CUIGif::OnUITimer()
{
	if (++m_nFrameIdx == m_vecElapses.size())
	{
		m_nFrameIdx = 0;

		if (!m_bLoop)
		{
			Stop();
			return;
		}
	}

	m_pImage->SelectActiveFrame(&Gdiplus::FrameDimensionTime, m_nFrameIdx);
	InvalidateRect(NULL);
	m_uiTimer.Start(m_vecElapses[m_nFrameIdx]);
}

void CUIGif::Start(bool bLoop)
{
	if (m_vecElapses.empty())
		return;

	m_bLoop = bLoop;
	m_uiTimer.Start(m_vecElapses[m_nFrameIdx]);
}

void CUIGif::Stop()
{
	m_uiTimer.Stop();
}

void CUIGif::Reset()
{
	if (m_nFrameIdx)
	{
		m_nFrameIdx = -1;
		OnUITimer();
	}

	Stop();
}
