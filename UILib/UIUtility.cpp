#include "stdafx.h"
#include "UIUtility.h"
#include "UIFontMgr.h"
#include "UIResource.h"

namespace UILib
{

UINT LoadSkin(LPCWSTR lpSkinName, bool bUpdate)
{
	return CUIResource::Get().LoadSkin(lpSkinName, bUpdate);
}

bool FreeSkin(UINT nSkinId, bool bUpdate)
{
	return CUIResource::Get().FreeSkin(nSkinId, bUpdate);
}

// 加载图片，格式为“xxx.png:n”，n<=4 为帧数，>4 为每帧宽度
CImagex GetImage(LPCWSTR lpFileName)
{
	wchar_t szFileName[MAX_PATH];
	int nWidth = 1;

	if (LPCWSTR lpFind = wcschr(lpFileName, ':'))
	{
		wcsncpy_s(szFileName, lpFileName, lpFind - lpFileName);
		lpFileName = szFileName;
		nWidth = _wtoi(lpFind + 1);
	}

	auto spImage = CUIResource::Get().GetImage(lpFileName);
	ATLASSERT(nWidth > 0 && nWidth <= spImage->GetWidth());

	if (!spImage || spImage->IsNull())
		return spImage;

	if (nWidth <= 1 || nWidth > spImage->GetWidth())
	{
		nWidth = spImage->GetWidth();
	}
	else if (nWidth <= 4)
	{
		ATLASSERT(spImage->GetWidth() % nWidth == 0);
		nWidth = spImage->GetWidth() / nWidth;
	}

	CImagex imagex;
	imagex.Reset(spImage, CRect(0, 0, nWidth, spImage->GetHeight()));
	return imagex;
}

CUIStream *GetStream(LPCWSTR lpFileName)
{
	return CUIResource::Get().GetStream(lpFileName);
}

bool HasAlphaChannel(const CImage &image)
{
#if 0
	return image.m_bHasAlphaChannel;
#else
	__asm
	{
		mov eax, image
		mov al, [eax + CImage::m_bHasAlphaChannel]
	}
#endif
}

void AlphaPng(CImage &image)
{
	if (image.GetBPP() != 32 || !HasAlphaChannel(image))
		return;

	LPBYTE pBegin = (LPBYTE)image.GetPixelAddress(0, image.GetPitch() < 0 ? image.GetHeight() - 1 : 0);
	LPBYTE pEnd   = pBegin + image.GetWidth() * image.GetHeight() * 4;

	for (LPBYTE pColor = pBegin; pColor != pEnd; pColor += 4)
	{
		if (pColor[3] != 255)
		{
			pColor[0] = (pColor[0] * pColor[3] + 127) / 255;
			pColor[1] = (pColor[1] * pColor[3] + 127) / 255;
			pColor[2] = (pColor[2] * pColor[3] + 127) / 255;
		}
	}
}

void FillAlpha(CImage &image, BYTE nAlpha)
{
	if (image.GetBPP() != 32)
		return;

	for (int x = 0; x != image.GetWidth(); x++)
	{
		for (int y = 0; y != image.GetHeight(); y++)
		{
			LPBYTE(image.GetPixelAddress(x, y))[3] = nAlpha;
		}
	}
}

void FillAlpha(CUIDC &dc, LPCRECT lpRect, BYTE nAlpha)
{
	if (!dc.IsLayered())
		return;

	CRect rect;
	GetClipBox(dc, rect);

	if (!rect.IntersectRect(rect, lpRect) || !dc.GetRealRect(rect))
		return;

	CImage image;
	image.Attach((HBITMAP)GetCurrentObject(dc, OBJ_BITMAP));

	for (int x = rect.left; x < rect.right; x++)
	{
		for (int y = rect.top; y < rect.bottom; y++)
		{
			LPBYTE(image.GetPixelAddress(x, y))[3] = nAlpha;
		}
	}

	image.Detach();
}

UINT SplitImage(LPCWSTR lpFileName, CImagex imagexs[], UINT nSize)
{
	if (nSize == 0)
		return 0;

	imagexs[0] = GetImage(lpFileName);
	UINT nCount = imagexs[0].GetFrameCount();

	if (nSize > nCount)
		nSize = nCount;

	for (UINT i = 1; i < nSize; i++)
	{
		imagexs[i] = imagexs[0];
		imagexs[i].SetFrameIndex(i);
	}

	return nSize;
}

void StretchDraw(HDC hdcDst, const CRect &rcDst, HDC hdcSrc, const CRect &rcSrc, void (*fnDraw)(HDC, int, int, int, int, HDC, int, int, int, int))
{
	int nWidth = rcDst.Width(), nHeight = rcDst.Height();
	int nSrcWidth = rcSrc.Width(), nSrcHeight = rcSrc.Height();

	// 大小相同
	if (nWidth == nSrcWidth && nHeight == nSrcHeight)
	{
		fnDraw(hdcDst, rcDst.left, rcDst.top, nWidth, nHeight, hdcSrc, rcSrc.left, rcSrc.top, nWidth, nHeight);
		return;
	}

	int nRightW = min(nWidth, nSrcWidth), nLeftW = nRightW / 2;
	nRightW -= nLeftW;

	int nBottomH = min(nHeight, nSrcHeight), nTopH = nBottomH / 2;
	nBottomH -= nTopH;

	// 高度相同
	if (nHeight == nSrcHeight)
	{
		fnDraw(hdcDst, rcDst.left, rcDst.top, nLeftW, nHeight, hdcSrc, rcSrc.left, rcSrc.top, nLeftW, nHeight);	// 左
		fnDraw(hdcDst, rcDst.right - nRightW, rcDst.top, nRightW, nHeight, hdcSrc, rcSrc.right - nRightW, rcSrc.top, nRightW, nHeight);	// 右

		if (nWidth > nSrcWidth)
			fnDraw(hdcDst, rcDst.left + nLeftW, rcDst.top, nWidth - nSrcWidth, nHeight, hdcSrc, rcSrc.left + nLeftW, rcSrc.top, 1, nHeight);
		return;
	}

	// 宽带相同
	if (nWidth == nSrcWidth)
	{
		fnDraw(hdcDst, rcDst.left, rcDst.top, nWidth, nTopH, hdcSrc, rcSrc.left, rcSrc.top, nWidth, nTopH);	// 上
		fnDraw(hdcDst, rcDst.left, rcDst.bottom - nBottomH, nWidth, nBottomH, hdcSrc, rcSrc.left, rcSrc.bottom - nBottomH, nWidth, nBottomH);	// 下

		if (nHeight > nSrcHeight)
			fnDraw(hdcDst, rcDst.left, rcDst.top + nTopH, nWidth, nHeight - nSrcHeight, hdcSrc, rcSrc.left, rcSrc.top + nTopH, nWidth, 1);
		return;
	}

	// 宽高都不同
	fnDraw(hdcDst, rcDst.left, rcDst.top, nLeftW, nTopH, hdcSrc, rcSrc.left, rcSrc.top, nLeftW, nTopH);	// 左上
	fnDraw(hdcDst, rcDst.right - nRightW, rcDst.top, nRightW, nTopH, hdcSrc, rcSrc.right - nRightW, rcSrc.top, nRightW, nTopH);	// 右上
	fnDraw(hdcDst, rcDst.left, rcDst.bottom - nBottomH, nLeftW, nBottomH, hdcSrc, rcSrc.left, rcSrc.bottom - nBottomH, nLeftW, nBottomH);	// 左下
	fnDraw(hdcDst, rcDst.right - nRightW, rcDst.bottom - nBottomH, nRightW, nBottomH, hdcSrc, rcSrc.right - nRightW, rcSrc.bottom - nBottomH, nRightW, nBottomH);	// 右下

	if (nWidth > nSrcWidth)
	{
		fnDraw(hdcDst, rcDst.left + nLeftW, rcDst.top, nWidth - nSrcWidth, nTopH, hdcSrc, rcSrc.left + nLeftW, rcSrc.top, 1, nTopH);	// 中上
		fnDraw(hdcDst, rcDst.left + nLeftW, rcDst.bottom - nBottomH, nWidth - nSrcWidth, nBottomH, hdcSrc, rcSrc.left + nLeftW, rcSrc.bottom - nBottomH, 1, nBottomH);	// 中下
	}

	if (nHeight > nSrcHeight)
	{
		fnDraw(hdcDst, rcDst.left, rcDst.top + nTopH, nLeftW, nHeight - nSrcHeight, hdcSrc, rcSrc.left, rcSrc.top + nTopH, nLeftW, 1);	// 左中
		fnDraw(hdcDst, rcDst.right - nRightW, rcDst.top + nTopH, nRightW, nHeight - nSrcHeight, hdcSrc, rcSrc.right - nRightW, rcSrc.top + nTopH, nRightW, 1);	// 右中
	}

	if (nWidth > nSrcWidth && nHeight > nSrcHeight)
		fnDraw(hdcDst, rcDst.left + nLeftW, rcDst.top + nTopH, nWidth - nSrcWidth, nHeight - nSrcHeight, hdcSrc, rcSrc.left + nLeftW, rcSrc.top + nTopH, 1, 1);	// 中间
}

void SetDefaultFont(LPCWSTR lpszName)
{
	CUIFontMgr::Get().SetFont(lpszName);
}

HFONT GetDefaultFont()
{
	static HFONT s_hFont = GetFont(NULL);
	return s_hFont;
}

HFONT GetFont(LPCWSTR lpszName, int nWeight, BYTE bItalic, BYTE bUnderline)
{
	return CUIFontMgr::Get().GetFont(lpszName, nWeight, bItalic, bUnderline);
}

void DrawImage3D(HDC hdcDst, int x, int y, int cx, int cy, HDC hdcSrc, int dy, int alpha)
{
	int nSavedDC = SaveDC(hdcDst);

	SetGraphicsMode(hdcDst, GM_ADVANCED);
	SetWindowOrgEx(hdcDst, -(x + cx / 2), -(y + cy / 2), NULL);

	XFORM xForm = {1, 0, 0, 1, 0, 0};
	xForm.eM11 = (float)abs(cos(alpha * 3.1415926535897932 / 180));
	double tmp = 4 * dy * (1 - xForm.eM11) / (cx * cy);

	for (int i = 0; i != cx; i++)
	{
		x = i - cx / 2;
		xForm.eM22 = (float)(1 - x * tmp);
		SetWorldTransform(hdcDst, &xForm);
		BitBlt(hdcDst, x, -cy / 2, 1, cy, hdcSrc, i, 0, SRCCOPY);
	}

	RestoreDC(hdcDst, nSavedDC);
}

void ScaleImage(const CImage &imgSrc, CImage &imgDst, int nWidth, int nHeight)
{
	Gdiplus::PixelFormat pixelFormat;

	switch (imgSrc.GetBPP())
	{
	case 32:
		pixelFormat = HasAlphaChannel(imgSrc) ? PixelFormat32bppARGB : PixelFormat32bppRGB;
		break;

	case 24:
		pixelFormat = PixelFormat24bppRGB;
		break;

	default:
		ATLASSERT(0);
		return;
	}

	if (!imgDst.Create(nWidth, nHeight, imgSrc.GetBPP(), pixelFormat == PixelFormat32bppARGB ? CImage::createAlphaChannel : 0))
		return;

	Gdiplus::Bitmap bmpSrc(imgSrc.GetWidth(), imgSrc.GetHeight(), imgSrc.GetPitch(), pixelFormat, (BYTE *)imgSrc.GetBits());
	Gdiplus::Bitmap bmpDst(imgDst.GetWidth(), imgDst.GetHeight(), imgDst.GetPitch(), pixelFormat, (BYTE *)imgDst.GetBits());

	Gdiplus::Graphics graphics(&bmpDst);
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	graphics.DrawImage(&bmpSrc, 0, 0, nWidth, nHeight);
}

void FormatPath(LPWSTR lpDstPath, LPCWSTR lpSrcPath)
{
	for (int ch; ch = *lpSrcPath++; *lpDstPath++ = ch)
	{
		if (ch >= 'A' && ch <= 'Z')
			ch += 'a' - 'A';
		else if (ch == '/')
			ch = '\\';
	}

	*lpDstPath = 0;
}

bool IsColorStr(LPCWSTR lpStr, COLORREF *pColor)
{
	if (lpStr == NULL || *lpStr != '#')
		return false;

	LPWSTR lpEnd = NULL;
	DWORD color = wcstoul(lpStr + 1, &lpEnd, 16);

	if (lpStr + 7 != lpEnd || *lpEnd)
		return false;

	if (pColor)
		*pColor = BswapRGB(color);

	return true;
}

DWORD BswapRGB(DWORD color)
{
	__asm
	{
		mov   eax, color
		shl   eax, 8
		bswap eax
	}
}

}	// namespace UILib
