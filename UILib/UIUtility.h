#pragma once

// 一些通用的方法

namespace UILib
{
UILIB_API UINT LoadSkin(LPCWSTR lpSkinName, bool bUpdate = false);
UILIB_API bool FreeSkin(UINT nSkinId, bool bUpdate = false);
UILIB_API CImagex GetImage(LPCWSTR lpFileName);
UILIB_API CUIStream *GetStream(LPCWSTR lpFileName);
UILIB_API bool LoadFromXml(LPCWSTR lpXmlName, CUIView *pRootView, IUILoadNotify *pLoadNotify);
UILIB_API bool HasAlphaChannel(const CImage &image);
UILIB_API void AlphaPng(CImage &image);
UILIB_API void FillAlpha(CImage &image, BYTE nAlpha);
UILIB_API void FillAlpha(CUIDC &dc, LPCRECT lpRect, BYTE nAlpha);
UILIB_API UINT SplitImage(LPCWSTR lpFileName, CImagex imagexs[], UINT nSize);
template <size_t nSize>
inline UINT SplitImage(LPCWSTR lpFileName, CImagex (&imagexs)[nSize]) { return SplitImage(lpFileName, imagexs, nSize); }
UILIB_API void StretchDraw(HDC hdcDst, const CRect &rcDst, HDC hdcSrc, const CRect &rcSrc, void (*fnDraw)(HDC, int, int, int, int, HDC, int, int, int, int));
UILIB_API void SetDefaultFont(LPCWSTR lpszName);
UILIB_API HFONT GetDefaultFont();
UILIB_API HFONT GetFont(LPCWSTR lpszName, int nWeight = FW_NORMAL, BYTE bItalic = FALSE, BYTE bUnderline = FALSE);
UILIB_API void DrawImage3D(HDC hdcDst, int x, int y, int cx, int cy, HDC hdcSrc, int dy, int alpha);
UILIB_API void ScaleImage(const CImage &imgSrc, CImage &imgDst, int nWidth, int nHeight);
UILIB_API void FormatPath(LPWSTR lpDstPath, LPCWSTR lpSrcPath);
UILIB_API bool IsColorStr(LPCWSTR lpStr, COLORREF *pColor);
UILIB_API DWORD BswapRGB(DWORD color);
}
