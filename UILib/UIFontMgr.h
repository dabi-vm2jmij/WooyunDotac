#pragma once

class CUIFontMgr
{
public:
	CUIFontMgr();
	~CUIFontMgr();

	struct FontKey
	{
		int     m_nHeight;
		int     m_nWeight;
		BYTE    m_bItalic;
		BYTE    m_bUnderline;
		wchar_t m_szFaceName[LF_FACESIZE];

		FontKey(HFONT hFont);
		FontKey(LPCWSTR lpszName, int nWeight, BYTE bItalic, BYTE bUnderline);
		void ParseName(LPCWSTR lpszName);
		bool operator<(const FontKey &_Right) const;
	};

	void  SetDefaultFont(LPCWSTR lpszName);
	HFONT GetCachedFont(LPCWSTR lpszName, int nWeight, BYTE bItalic, BYTE bUnderline);
	HFONT GetCachedFont(const FontKey &fontKey);

private:
	FontKey m_fontKey;
	CRITICAL_SECTION m_critSect;
	std::map<FontKey, HFONT> m_mapFonts;
};
