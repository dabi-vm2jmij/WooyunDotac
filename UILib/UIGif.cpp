#include "stdafx.h"
#include "UIGif.h"

CUIGif::CUIGif(CUIView *pParent, LPCWSTR lpFileName) : CUIControl(pParent), m_pImage(NULL), m_bLoop(false), m_nFrameIdx(0), m_uiTimer([this]{ OnUITimer(); })
{
	m_bClickable = false;

	IStream *pStream;
	if (lpFileName && (pStream = ::GetStream(lpFileName)))
	{
		Init(pStream);
		pStream->Release();
	}
}

void CUIGif::Init(IStream *pStream)
{
	if (m_pImage)
	{
		ATLASSERT(0);
		return;
	}

	if (pStream)
		m_pImage = new Gdiplus::Image(pStream);

	if (m_pImage == NULL || m_pImage->GetLastStatus() != Gdiplus::Ok)
	{
		ATLASSERT(0);
		return;
	}

	SetSize(CSize(m_pImage->GetWidth(), m_pImage->GetHeight()));

	UINT nCount = m_pImage->GetFrameDimensionsCount();
	GUID *pDimensionIDs = (GUID *)alloca(sizeof(GUID) * nCount + 1);

	if (nCount == 0 || m_pImage->GetFrameDimensionsList(pDimensionIDs, nCount) != Gdiplus::Ok || (nCount = m_pImage->GetFrameCount(&pDimensionIDs[0])) == 0)
		return;

	UINT nSize = m_pImage->GetPropertyItemSize(PropertyTagFrameDelay);
	Gdiplus::PropertyItem *pPropItem = (Gdiplus::PropertyItem *)alloca(nSize + 1);

	if (nSize == 0 || m_pImage->GetPropertyItem(PropertyTagFrameDelay, nSize, pPropItem) != Gdiplus::Ok)
		return;

	m_vecElapses.reserve(nCount);

	for (int i = 0; i != nCount; i++)
	{
		UINT nElapse = ((UINT *)pPropItem->value)[i] * 10;
		m_vecElapses.push_back(nElapse ? nElapse : 100);
	}
}

CUIGif::~CUIGif()
{
	if (m_pImage)
		delete m_pImage;
}

void CUIGif::OnPaint(CUIDC &dc) const
{
	__super::OnPaint(dc);

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
	InvalidateRect();
	m_uiTimer.Start(m_vecElapses[m_nFrameIdx]);
}

void CUIGif::Start(bool bLoop)
{
	if (m_vecElapses.size() < 2)
	{
		ATLASSERT(0);
		return;
	}

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

void CUIGif::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	if (attrs.GetInt(L"start"))
		Start();
}
