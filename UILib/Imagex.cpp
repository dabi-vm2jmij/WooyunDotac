#include "stdafx.h"
#include "Imagex.h"

CImagex::CImagex(const CImagePtr &spImage) : m_spImage(spImage)
{
	SetRect();
}

CImagex::CImagex(CImagePtr &&spImage) : m_spImage(std::move(spImage))
{
	SetRect();
}

CImagex::CImagex(CImagex &&_Right)
{
	Swap(_Right);
}

CImagex &CImagex::operator=(const CImagePtr &spImage)
{
	CImagex(spImage).Swap(*this);
	return *this;
}

CImagex &CImagex::operator=(CImagePtr &&spImage)
{
	CImagex(std::move(spImage)).Swap(*this);
	return *this;
}

CImagex &CImagex::operator=(const CImagex &_Right)
{
	CImagex(_Right).Swap(*this);
	return *this;
}

CImagex &CImagex::operator=(CImagex &&_Right)
{
	CImagex(std::move(_Right)).Swap(*this);
	return *this;
}

bool CImagex::operator==(const CImagex &_Right) const
{
	return m_spImage == _Right.m_spImage && m_rect == _Right.m_rect;
}

bool CImagex::operator!=(const CImagex &_Right) const
{
	return !(*this == _Right);
}

CImagex::operator HBITMAP() const
{
	return m_spImage ? (HBITMAP)Image() : NULL;
}

const CImage &CImagex::Image() const
{
	return *m_spImage;
}

const CRect &CImagex::Rect() const
{
	return m_rect;
}

void CImagex::Swap(CImagex &_Other)
{
	m_spImage.swap(_Other.m_spImage);
	std::swap(m_rect, _Other.m_rect);
}

void CImagex::Reset(const CImagePtr &spImage, const CRect &rect)
{
	if (m_spImage != spImage)
		m_spImage = spImage;

	if (*this)
	{
		ATLASSERT(rect.left >= 0 && rect.left < rect.right && rect.right <= m_spImage->GetWidth());
		ATLASSERT(rect.top >= 0 && rect.top < rect.bottom && rect.bottom <= m_spImage->GetHeight());
		m_rect = rect;
	}
	else
		m_rect = CRect();
}

UINT CImagex::GetFrameCount() const
{
	return *this ? m_spImage->GetWidth() / m_rect.Width() : 0;
}

void CImagex::SetFrameIndex(UINT nIndex)
{
	m_rect.MoveToX(m_rect.Width() * nIndex);
}

void CImagex::SetRect()
{
	if (*this)
		m_rect = CRect(0, 0, m_spImage->GetWidth(), m_spImage->GetHeight());
	else
		m_rect = CRect();
}

void CImagex::BitBlt(HDC hdcDst, int xDst, int yDst, int nDstWidth, int nDstHeight, int xSrc, int ySrc, DWORD dwROP) const
{
	ATLASSERT(!m_spImage->IsNull());
	ATLASSERT(nDstWidth > 0 && nDstHeight > 0);

	::BitBlt(hdcDst, xDst, yDst, nDstWidth, nDstHeight, CUIComDC(Image()), xSrc, ySrc, dwROP);
}

void CImagex::BitBlt(HDC hdcDst, const CRect &rcDst, const CPoint &ptSrc, DWORD dwROP) const
{
	return BitBlt(hdcDst, rcDst.left, rcDst.top, rcDst.Width(), rcDst.Height(), ptSrc.x, ptSrc.y, dwROP);
}

void CImagex::BitBlt(HDC hdcDst, int xDst, int yDst, DWORD dwROP) const
{
	return BitBlt(hdcDst, xDst, yDst, m_rect.Width(), m_rect.Height(), m_rect.left, m_rect.top, dwROP);
}

void CImagex::BitBlt(HDC hdcDst, const CPoint &ptDst, DWORD dwROP) const
{
	return BitBlt(hdcDst, ptDst.x, ptDst.y, dwROP);
}

void CImagex::Draw(HDC hdcDst, int xDst, int yDst, int nDstWidth, int nDstHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight) const
{
	ATLASSERT(!m_spImage->IsNull());
	ATLASSERT(nDstWidth > 0 && nDstHeight > 0 && nSrcWidth > 0 && nSrcHeight > 0);

	if (HasAlphaChannel(Image()))
	{
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		::AlphaBlend(hdcDst, xDst, yDst, nDstWidth, nDstHeight, CUIComDC(Image()), xSrc, ySrc, nSrcWidth, nSrcHeight, bf);
	}
	else
		::StretchBlt(hdcDst, xDst, yDst, nDstWidth, nDstHeight, CUIComDC(Image()), xSrc, ySrc, nSrcWidth, nSrcHeight, SRCCOPY);
}

void CImagex::Draw(HDC hdcDst, const CRect &rcDst, const CRect &rcSrc) const
{
	Draw(hdcDst, rcDst.left, rcDst.top, rcDst.Width(), rcDst.Height(), rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height());
}

void CImagex::Draw(HDC hdcDst, int xDst, int yDst, int nDstWidth, int nDstHeight) const
{
	Draw(hdcDst, xDst, yDst, nDstWidth, nDstHeight, m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height());
}

void CImagex::Draw(HDC hdcDst, const CRect &rcDst) const
{
	Draw(hdcDst, rcDst.left, rcDst.top, rcDst.Width(), rcDst.Height());
}

void CImagex::Draw(HDC hdcDst, int xDst, int yDst) const
{
	Draw(hdcDst, xDst, yDst, m_rect.Width(), m_rect.Height());
}

void CImagex::Draw(HDC hdcDst, const CPoint &ptDst) const
{
	Draw(hdcDst, ptDst.x, ptDst.y);
}

void CImagex::AlphaBlend(HDC hdcDst, int xDst, int yDst, int nDstWidth, int nDstHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, BYTE nSrcAlpha) const
{
	ATLASSERT(!m_spImage->IsNull());
	ATLASSERT(nDstWidth > 0 && nDstHeight > 0 && nSrcWidth > 0 && nSrcHeight > 0);

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, nSrcAlpha, AC_SRC_ALPHA };
	::AlphaBlend(hdcDst, xDst, yDst, nDstWidth, nDstHeight, CUIComDC(Image()), xSrc, ySrc, nSrcWidth, nSrcHeight, bf);
}

void CImagex::AlphaBlend(HDC hdcDst, const CRect &rcDst, const CRect &rcSrc, BYTE nSrcAlpha) const
{
	AlphaBlend(hdcDst, rcDst.left, rcDst.top, rcDst.Width(), rcDst.Height(), rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(), nSrcAlpha);
}

void CImagex::AlphaBlend(HDC hdcDst, int xDst, int yDst, BYTE nSrcAlpha) const
{
	AlphaBlend(hdcDst, xDst, yDst, m_rect.Width(), m_rect.Height(), m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), nSrcAlpha);
}

void CImagex::AlphaBlend(HDC hdcDst, const CPoint &ptDst, BYTE nSrcAlpha) const
{
	AlphaBlend(hdcDst, ptDst.x, ptDst.y, nSrcAlpha);
}

void CImagex::StretchBlt(HDC hdcDst, int xDst, int yDst, int nDstWidth, int nDstHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwROP) const
{
	ATLASSERT(!m_spImage->IsNull());
	ATLASSERT(nDstWidth > 0 && nDstHeight > 0 && nSrcWidth > 0 && nSrcHeight > 0);

	::StretchBlt(hdcDst, xDst, yDst, nDstWidth, nDstHeight, CUIComDC(Image()), xSrc, ySrc, nSrcWidth, nSrcHeight, dwROP);
}

void CImagex::StretchBlt(HDC hdcDst, const CRect &rcDst, const CRect &rcSrc, DWORD dwROP) const
{
	StretchBlt(hdcDst, rcDst.left, rcDst.top, rcDst.Width(), rcDst.Height(), rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(), dwROP);
}

void CImagex::StretchBlt(HDC hdcDst, int xDst, int yDst, int nDstWidth, int nDstHeight, DWORD dwROP) const
{
	StretchBlt(hdcDst, xDst, yDst, nDstWidth, nDstHeight, m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), dwROP);
}

void CImagex::StretchBlt(HDC hdcDst, const CRect &rcDst, DWORD dwROP) const
{
	StretchBlt(hdcDst, rcDst.left, rcDst.top, rcDst.Width(), rcDst.Height(), dwROP);
}

// «–≥…æ≈π¨∏ÒªÊ÷∆
void CImagex::StretchDraw(HDC hdcDst, const CRect &rcDst, bool bAlpha) const
{
	void (*fnDraw)(HDC, int, int, int, int, HDC, int, int, int, int);

	if (bAlpha && HasAlphaChannel(Image()))
	{
		fnDraw = [](HDC hdcDst, int xDst, int yDst, int wDst, int hDst, HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc)
		{
			BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
			::AlphaBlend(hdcDst, xDst, yDst, wDst, hDst, hdcSrc, xSrc, ySrc, wSrc, hSrc, bf);
		};
	}
	else
	{
		fnDraw = [](HDC hdcDst, int xDst, int yDst, int wDst, int hDst, HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc)
		{
			::StretchBlt(hdcDst, xDst, yDst, wDst, hDst, hdcSrc, xSrc, ySrc, wSrc, hSrc, SRCCOPY);
		};
	}

	::StretchDraw(hdcDst, rcDst, CUIComDC(Image()), Rect(), fnDraw);
}
