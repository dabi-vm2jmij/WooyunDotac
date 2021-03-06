#pragma once

// 一些通用的方法

namespace UILib
{

UILIB_API UINT LoadSkin(LPCWSTR lpSkinName, bool bUpdate = false);
UILIB_API bool FreeSkin(UINT nSkinId, bool bUpdate = false);
UILIB_API CImagex GetImage(LPCWSTR lpFileName);
UILIB_API IUIStream *GetStream(LPCWSTR lpFileName);
UILIB_API bool HasAlphaChannel(const CImage &image);
UILIB_API void AlphaPng(CImage &image);
UILIB_API void Scale9Draw(HDC hdcDst, const CRect &rcDst, HDC hdcSrc, const CRect &rcSrc, void (*fnDraw)(HDC, int, int, int, int, HDC, int, int, int, int));
UILIB_API void SetDefaultFont(LPCWSTR lpszName);
UILIB_API HFONT GetDefaultFont();
UILIB_API HFONT GetCachedFont(LPCWSTR lpszName, int nWeight = FW_NORMAL, BYTE bItalic = FALSE, BYTE bUnderline = FALSE);
UILIB_API void ScaleImage(const CImage &imgSrc, CImage &imgDst, int nWidth, int nHeight);
UILIB_API void FormatPath(LPWSTR lpDstPath, LPCWSTR lpSrcPath);
UILIB_API bool StrToColor(LPCWSTR lpStr, COLORREF &color);
UILIB_API DWORD BswapRGB(DWORD color);

}
