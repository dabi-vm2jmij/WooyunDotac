#pragma once

class CUISkin;

// ��������ͼƬ��Դ

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
		UINT nSkinId;	// �� wpImage ��Чʱ����Ӧ�� nSkinId
		std::weak_ptr<CImage> wpImage;
	};

	std::map<wstring, ImgInfo> m_mapImages;	// ���� CImage �������ã�û��ǿ����ʱ�Զ� delete CImage
};
