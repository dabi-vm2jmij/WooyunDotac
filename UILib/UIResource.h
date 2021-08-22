#pragma once

class CUISkin;

// ��������ͼƬ��Դ

class CUIResource
{
	CUIResource();
	~CUIResource();

public:
	static CUIResource &Get();

	CUIResPath *GetResPath() const;
	void SetResPath(CUIResPath *pResPath) const;

	UINT LoadSkin(LPCWSTR lpSkinName, bool bUpdate);
	bool FreeSkin(UINT nSkinId, bool bUpdate);

	CImagePtr GetImage(LPCWSTR lpFileName);
	CUIStream *GetStream(LPCWSTR lpFileName);

private:
	void UpdateImages();
	CUIStream *FindStream(LPCWSTR lpFileName) const;

	DWORD m_dwTlsIndex;
	CRITICAL_SECTION m_critSect;
	std::vector<CUISkin *> m_vecSkins;

	struct ImgInfo
	{
		UINT nSkinId;	// �� wpImage ��Чʱ����Ӧ�� nSkinId
		std::weak_ptr<CImage> wpImage;
	};

	std::map<wstring, ImgInfo> m_mapImages;	// ���� CImage �������ã�û��ǿ����ʱ�Զ� delete CImage
};