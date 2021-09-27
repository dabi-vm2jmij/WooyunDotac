#include "stdafx.h"
#include "UIUtility.h"
#include "UILibApp.h"

namespace UILib
{

UINT LoadSkin(LPCWSTR lpSkinName, bool bUpdate)
{
	return g_theApp.GetResource().LoadSkin(lpSkinName, bUpdate);
}

bool FreeSkin(UINT nSkinId, bool bUpdate)
{
	return g_theApp.GetResource().FreeSkin(nSkinId, bUpdate);
}

// ����ͼƬ����ʽΪ��xxx.png:n����n Ϊ֡��
CImagex GetImage(LPCWSTR lpFileName)
{
	if (lpFileName == NULL || *lpFileName == 0)
	{
		ATLASSERT(0);
		return nullptr;
	}

	wchar_t szFileName[MAX_PATH];
	int nCount = 1;

	if (LPCWSTR lpFind = wcschr(lpFileName, ':'))
	{
		wcsncpy_s(szFileName, lpFileName, lpFind - lpFileName);
		lpFileName = szFileName;
		nCount = _wtoi(lpFind + 1);
	}

	auto spImage = g_theApp.GetResource().GetImage(lpFileName);
	ATLASSERT(nCount > 0 && spImage->GetWidth() % nCount == 0);

	if (!spImage || spImage->IsNull() || nCount <= 1)
		return spImage;

	CImagex imagex;
	imagex.Reset(spImage, CRect(0, 0, spImage->GetWidth() / nCount, spImage->GetHeight()));
	return imagex;
}

CUIStream *GetStream(LPCWSTR lpFileName)
{
	if (lpFileName == NULL || *lpFileName == 0)
	{
		ATLASSERT(0);
		return nullptr;
	}

	return g_theApp.GetResource().GetStream(lpFileName);
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

UINT SplitImage(LPCWSTR lpFileName, CImagex imagexs[], UINT nCount)
{
	if (nCount == 0)
		return 0;

	imagexs[0] = GetImage(lpFileName);
	UINT nTotal = imagexs[0].GetFrameCount();

	if (nCount > nTotal)
		nCount = nTotal;

	for (UINT i = 1; i < nCount; i++)
	{
		imagexs[i] = imagexs[0];
		imagexs[i].SetFrameIndex(i);
	}

	return nCount;
}

void StretchDraw(HDC hdcDst, const CRect &rcDst, HDC hdcSrc, const CRect &rcSrc, void (*fnDraw)(HDC, int, int, int, int, HDC, int, int, int, int))
{
	int nWidth = rcDst.Width(), nHeight = rcDst.Height();
	int nSrcWidth = rcSrc.Width(), nSrcHeight = rcSrc.Height();

	// ��С��ͬ
	if (nWidth == nSrcWidth && nHeight == nSrcHeight)
	{
		fnDraw(hdcDst, rcDst.left, rcDst.top, nWidth, nHeight, hdcSrc, rcSrc.left, rcSrc.top, nWidth, nHeight);
		return;
	}

	int nRightW = min(nWidth, nSrcWidth), nLeftW = nRightW / 2;
	nRightW -= nLeftW;

	int nBottomH = min(nHeight, nSrcHeight), nTopH = nBottomH / 2;
	nBottomH -= nTopH;

	// �߶���ͬ
	if (nHeight == nSrcHeight)
	{
		fnDraw(hdcDst, rcDst.left, rcDst.top, nLeftW, nHeight, hdcSrc, rcSrc.left, rcSrc.top, nLeftW, nHeight);	// ��
		fnDraw(hdcDst, rcDst.right - nRightW, rcDst.top, nRightW, nHeight, hdcSrc, rcSrc.right - nRightW, rcSrc.top, nRightW, nHeight);	// ��

		if (nWidth > nSrcWidth)
			fnDraw(hdcDst, rcDst.left + nLeftW, rcDst.top, nWidth - nSrcWidth, nHeight, hdcSrc, rcSrc.left + nLeftW, rcSrc.top, 1, nHeight);
		return;
	}

	// �����ͬ
	if (nWidth == nSrcWidth)
	{
		fnDraw(hdcDst, rcDst.left, rcDst.top, nWidth, nTopH, hdcSrc, rcSrc.left, rcSrc.top, nWidth, nTopH);	// ��
		fnDraw(hdcDst, rcDst.left, rcDst.bottom - nBottomH, nWidth, nBottomH, hdcSrc, rcSrc.left, rcSrc.bottom - nBottomH, nWidth, nBottomH);	// ��

		if (nHeight > nSrcHeight)
			fnDraw(hdcDst, rcDst.left, rcDst.top + nTopH, nWidth, nHeight - nSrcHeight, hdcSrc, rcSrc.left, rcSrc.top + nTopH, nWidth, 1);
		return;
	}

	// ��߶���ͬ
	fnDraw(hdcDst, rcDst.left, rcDst.top, nLeftW, nTopH, hdcSrc, rcSrc.left, rcSrc.top, nLeftW, nTopH);	// ����
	fnDraw(hdcDst, rcDst.right - nRightW, rcDst.top, nRightW, nTopH, hdcSrc, rcSrc.right - nRightW, rcSrc.top, nRightW, nTopH);	// ����
	fnDraw(hdcDst, rcDst.left, rcDst.bottom - nBottomH, nLeftW, nBottomH, hdcSrc, rcSrc.left, rcSrc.bottom - nBottomH, nLeftW, nBottomH);	// ����
	fnDraw(hdcDst, rcDst.right - nRightW, rcDst.bottom - nBottomH, nRightW, nBottomH, hdcSrc, rcSrc.right - nRightW, rcSrc.bottom - nBottomH, nRightW, nBottomH);	// ����

	if (nWidth > nSrcWidth)
	{
		fnDraw(hdcDst, rcDst.left + nLeftW, rcDst.top, nWidth - nSrcWidth, nTopH, hdcSrc, rcSrc.left + nLeftW, rcSrc.top, 1, nTopH);	// ����
		fnDraw(hdcDst, rcDst.left + nLeftW, rcDst.bottom - nBottomH, nWidth - nSrcWidth, nBottomH, hdcSrc, rcSrc.left + nLeftW, rcSrc.bottom - nBottomH, 1, nBottomH);	// ����
	}

	if (nHeight > nSrcHeight)
	{
		fnDraw(hdcDst, rcDst.left, rcDst.top + nTopH, nLeftW, nHeight - nSrcHeight, hdcSrc, rcSrc.left, rcSrc.top + nTopH, nLeftW, 1);	// ����
		fnDraw(hdcDst, rcDst.right - nRightW, rcDst.top + nTopH, nRightW, nHeight - nSrcHeight, hdcSrc, rcSrc.right - nRightW, rcSrc.top + nTopH, nRightW, 1);	// ����
	}

	if (nWidth > nSrcWidth && nHeight > nSrcHeight)
		fnDraw(hdcDst, rcDst.left + nLeftW, rcDst.top + nTopH, nWidth - nSrcWidth, nHeight - nSrcHeight, hdcSrc, rcSrc.left + nLeftW, rcSrc.top + nTopH, 1, 1);	// �м�
}

void SetDefaultFont(LPCWSTR lpszName)
{
	g_theApp.GetFontMgr().SetFont(lpszName);
}

HFONT GetDefaultFont()
{
	return GetFont(NULL);
}

HFONT GetFont(LPCWSTR lpszName, int nWeight, BYTE bItalic, BYTE bUnderline)
{
	return g_theApp.GetFontMgr().GetFont(lpszName, nWeight, bItalic, bUnderline);
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

bool StrToColor(LPCWSTR lpStr, COLORREF &color)
{
	if (lpStr == NULL || *lpStr != '#')
		return false;

	LPWSTR lpEnd = NULL;
	DWORD value = wcstoul(++lpStr, &lpEnd, 16);

	if (lpStr + 6 != lpEnd || *lpEnd)
		return false;

	color = BswapRGB(value);
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
