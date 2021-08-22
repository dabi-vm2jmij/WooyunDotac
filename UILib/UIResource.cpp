#include "stdafx.h"
#include "UIResource.h"
#include "UISkin.h"

CUIResource::CUIResource() : m_dwTlsIndex(TlsAlloc())
{
	InitializeCriticalSection(&m_critSect);
}

CUIResource::~CUIResource()
{
	for (auto pSkin : m_vecSkins)
		pSkin->Release();

	TlsFree(m_dwTlsIndex);
	DeleteCriticalSection(&m_critSect);
}

CUIResource &CUIResource::Get()
{
	static CUIResource s_resource;
	return s_resource;
}

CUIResPath *CUIResource::GetResPath() const
{
	return (CUIResPath *)TlsGetValue(m_dwTlsIndex);
}

void CUIResource::SetResPath(CUIResPath *pResPath) const
{
	TlsSetValue(m_dwTlsIndex, pResPath);
}

UINT CUIResource::LoadSkin(LPCWSTR lpSkinName, bool bUpdate)
{
	CUISkin *pSkin = new CUISkin(lpSkinName);
	EnterCriticalSection(&m_critSect);
	m_vecSkins.push_back(pSkin);

	if (bUpdate)
		UpdateImages();

	LeaveCriticalSection(&m_critSect);
	return (UINT)pSkin;
}

bool CUIResource::FreeSkin(UINT nSkinId, bool bUpdate)
{
	EnterCriticalSection(&m_critSect);

	for (auto it = m_vecSkins.begin(); it != m_vecSkins.end(); ++it)
	{
		CUISkin *pSkin = *it;

		if ((UINT)pSkin == nSkinId)
		{
			m_vecSkins.erase(it);
			pSkin->Release();

			if (bUpdate)
				UpdateImages();

			LeaveCriticalSection(&m_critSect);
			return true;
		}
	}

	LeaveCriticalSection(&m_critSect);
	return false;
}

static UINT &SkinIdOfStream(CUIStream *pStream)
{
#if 0
	return pStream->m_nSkinId;
#else
	__asm
	{
		mov eax, pStream
		add eax, CUIStream::m_nSkinId
	}
#endif
}

CImagePtr CUIResource::GetImage(LPCWSTR lpFileName)
{
	wchar_t szFileName[MAX_PATH];
	COLORREF color = -1;

	if (IsColorStr(lpFileName, &color))
	{
		wcscpy_s(szFileName, lpFileName);
		CharLowerW(szFileName);
	}
	else
		GetResPath()->CombinePath(szFileName, lpFileName);

	EnterCriticalSection(&m_critSect);
	auto &imgInfo = m_mapImages[szFileName];
	auto  spImage = imgInfo.wpImage.lock();

	if (!spImage)
	{
		imgInfo.nSkinId = 0;
		spImage = std::make_shared<CImage>();

		if (color != -1)
		{
			if (spImage->Create(1, 1, 32))
				*(int *)spImage->GetPixelAddress(0, 0) = BswapRGB(color) | 0xff000000;
		}
		else if (auto pStream = FindStream(szFileName))
		{
			if (spImage->Load(pStream) == S_OK)
			{
				AlphaPng(*spImage);
				imgInfo.nSkinId = SkinIdOfStream(pStream);
			}

			pStream->Release();
		}

		imgInfo.wpImage = spImage;
	}

	LeaveCriticalSection(&m_critSect);
	return spImage;
}

// 加载或释放皮肤后，更新所有相关的图片
void CUIResource::UpdateImages()
{
	for (auto &it : m_mapImages)
	{
		if (IsColorStr(it.first.c_str(), NULL))
			continue;

		auto &imgInfo = it.second;
		auto  spImage = imgInfo.wpImage.lock();

		if (!spImage)
			continue;

		if (auto pStream = FindStream(it.first.c_str()))
		{
			// nSkinId 不同才更新
			if (imgInfo.nSkinId != SkinIdOfStream(pStream))
			{
				imgInfo.nSkinId = 0;
				spImage->Destroy();

				if (spImage->Load(pStream) == S_OK)
				{
					AlphaPng(*spImage);
					imgInfo.nSkinId = SkinIdOfStream(pStream);
				}
			}

			pStream->Release();
		}
	}
}

CUIStream *CUIResource::GetStream(LPCWSTR lpFileName)
{
	wchar_t szFileName[MAX_PATH];
	GetResPath()->CombinePath(szFileName, lpFileName);

	EnterCriticalSection(&m_critSect);
	auto pStream = FindStream(szFileName);
	LeaveCriticalSection(&m_critSect);
	return pStream;
}

CUIStream *CUIResource::FindStream(LPCWSTR lpFileName) const
{
	for (auto it = m_vecSkins.rbegin(); it != m_vecSkins.rend(); ++it)
	{
		CUISkin *pSkin = *it;

		if (auto pStream = pSkin->GetStream(lpFileName))
		{
			pSkin->AddRef();
			SkinIdOfStream(pStream) = (UINT)pSkin;
			return pStream;
		}
	}

	return NULL;
}
