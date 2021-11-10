#pragma once

class CUISkin;

// 管理所有图片资源

class CUIResource
{
public:
	CUIResource();
	~CUIResource();

	CUIResPath *GetResPath() const;
	void SetResPath(CUIResPath *pResPath) const;

	UINT LoadSkin(LPCWSTR lpSkinName, bool bUpdate);
	bool FreeSkin(UINT nSkinId, bool bUpdate);

	CImagePtr GetImage(LPCWSTR lpFileName);
	IUIStream *GetStream(LPCWSTR lpFileName);

private:
	void UpdateImages();
	IUIStream *FindStream(LPCWSTR lpFileName) const;

	DWORD m_dwTlsIndex;
	CRITICAL_SECTION m_critSect;
	vector<std::shared_ptr<CUISkin>> m_vecSkins;

	struct ImgInfo
	{
		UINT nSkinId;	// 当 wpImage 有效时，对应的 nSkinId
		std::weak_ptr<CImage> wpImage;
	};

	std::map<wstring, ImgInfo> m_mapImages;	// 保存 CImage 的弱引用，没有强引用时自动 delete CImage
};
