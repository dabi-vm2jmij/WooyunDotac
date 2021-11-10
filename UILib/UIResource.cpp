#include "stdafx.h"
#include "UIResource.h"
#include "UISkin.h"

CUIResource::CUIResource() : m_dwTlsIndex(TlsAlloc())
{
	InitializeCriticalSection(&m_critSect);
}

CUIResource::~CUIResource()
{
	TlsFree(m_dwTlsIndex);
	DeleteCriticalSection(&m_critSect);
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
	auto spSkin = std::make_shared<CUISkin>(lpSkinName);
	EnterCriticalSection(&m_critSect);
	m_vecSkins.push_back(spSkin);

	if (bUpdate)
		UpdateImages();

	LeaveCriticalSection(&m_critSect);
	return spSkin->GetId();
}

bool CUIResource::FreeSkin(UINT nSkinId, bool bUpdate)
{
	EnterCriticalSection(&m_critSect);

	for (auto it = m_vecSkins.begin(); it != m_vecSkins.end(); ++it)
	{
		if ((*it)->GetId() == nSkinId)
		{
			m_vecSkins.erase(it);

			if (bUpdate)
				UpdateImages();

			LeaveCriticalSection(&m_critSect);
			return true;
		}
	}

	LeaveCriticalSection(&m_critSect);
	return false;
}

static std::shared_ptr<CUISkin> &SkinOfStream(IUIStream *pStream)
{
#if 0
	return ((CUIStream *)pStream)->m_spSkin;
#else
	__asm
	{
		mov eax, pStream
		add eax, CUIStream::m_spSkin
	}
#endif
}

static UINT SkinIdOfStream(IUIStream *pStream)
{
	auto &spSkin = SkinOfStream(pStream);
	return spSkin ? spSkin->GetId() : 0;
}

CImagePtr CUIResource::GetImage(LPCWSTR lpFileName)
{
	wchar_t szFileName[MAX_PATH];
	GetResPath()->CombinePath(szFileName, lpFileName);

	EnterCriticalSection(&m_critSect);
	auto &imgInfo = m_mapImages[szFileName];
	auto  spImage = imgInfo.wpImage.lock();

	if (!spImage)
	{
		imgInfo.nSkinId = 0;
		spImage = std::make_shared<CImage>();

		if (auto pStream = FindStream(szFileName))
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
	for (auto it = m_mapImages.begin(); it != m_mapImages.end(); )
	{
		auto &imgInfo = it->second;
		auto  spImage = imgInfo.wpImage.lock();

		if (!spImage)
		{
			++it;
			continue;
		}

		if (auto pStream = FindStream(it->first.c_str()))
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
			++it;
		}
		else
			it = m_mapImages.erase(it);
	}
}

IUIStream *CUIResource::GetStream(LPCWSTR lpFileName)
{
	wchar_t szFileName[MAX_PATH];
	GetResPath()->CombinePath(szFileName, lpFileName);

	EnterCriticalSection(&m_critSect);
	auto pStream = FindStream(szFileName);
	LeaveCriticalSection(&m_critSect);
	return pStream;
}

IUIStream *CUIResource::FindStream(LPCWSTR lpFileName) const
{
	for (auto it = m_vecSkins.rbegin(); it != m_vecSkins.rend(); ++it)
	{
		if (auto pStream = (*it)->GetStream(lpFileName))
		{
			SkinOfStream(pStream) = *it;
			return pStream;
		}
	}

	return NULL;
}
