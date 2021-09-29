#include "stdafx.h"
#include "UILoader.h"
#include "tinyxml2.h"

namespace
{

class CUILoadData
{
public:
	void     SetFont(LPCWSTR lpszId, HFONT hFont);
	void     SetView(LPCWSTR lpszId, CUIView *pView);
	HFONT    GetFont(LPCWSTR lpszId) const;
	CUIView *GetView(LPCWSTR lpszId) const;

private:
	std::map<wstring, HFONT>     m_mapFonts;
	std::map<wstring, CUIView *> m_mapViews;
};

class CUILoadAttrs : public IUILoadAttrs
{
public:
	CUILoadAttrs(const CUILoadData &loadData, tinyxml2::XMLElement *pElem);

	virtual LPCWSTR  GetStr(LPCWSTR lpName) const override;
	virtual bool     GetInt(LPCWSTR lpName, int *pnValue) const override;
	virtual HFONT    GetFont(LPCWSTR lpszId) const override;
	virtual CUIView *GetView(LPCWSTR lpszId) const override;

private:
	const CUILoadData &m_loadData;
	std::map<wstring, wstring> m_mapAttrs;
};

wstring StrLower(LPCWSTR lpStr)
{
	wstring strRet(lpStr);
	CharLowerBuffW((LPWSTR)strRet.c_str(), strRet.size());
	return strRet;
}

void CUILoadData::SetFont(LPCWSTR lpszId, HFONT hFont)
{
	ATLVERIFY(m_mapFonts.emplace(StrLower(lpszId), hFont).second);
}

void CUILoadData::SetView(LPCWSTR lpszId, CUIView *pView)
{
	ATLVERIFY(m_mapViews.emplace(StrLower(lpszId), pView).second);
}

HFONT CUILoadData::GetFont(LPCWSTR lpszId) const
{
	auto it = m_mapFonts.find(StrLower(lpszId));
	return it != m_mapFonts.end() ? it->second : NULL;
}

CUIView *CUILoadData::GetView(LPCWSTR lpszId) const
{
	auto it = m_mapViews.find(StrLower(lpszId));
	return it != m_mapViews.end() ? it->second : NULL;
}

CUILoadAttrs::CUILoadAttrs(const CUILoadData &loadData, tinyxml2::XMLElement *pElem) : m_loadData(loadData)
{
	for (const tinyxml2::XMLAttribute *pAttr = pElem->FirstAttribute(); pAttr; pAttr = pAttr->Next())
	{
		m_mapAttrs.emplace(CharLowerW(CA2W(pAttr->Name(), CP_UTF8)), (LPCWSTR)CA2W(pAttr->Value(), CP_UTF8));
	}
}

LPCWSTR CUILoadAttrs::GetStr(LPCWSTR lpName) const
{
	auto it = m_mapAttrs.find(StrLower(lpName));
	return it != m_mapAttrs.end() ? it->second.c_str() : NULL;
}

bool CUILoadAttrs::GetInt(LPCWSTR lpName, int *pnValue) const
{
	LPCWSTR lpStr = GetStr(lpName);
	if (lpStr == NULL)
		return false;

	LPCWSTR lpEnd = lpStr;
	int nValue = wcstoul(lpStr, (LPWSTR *)&lpEnd, 0);

	if (lpStr == lpEnd || *lpEnd)
	{
		ATLASSERT(0);
		return false;
	}

	*pnValue = nValue;
	return true;
}

HFONT CUILoadAttrs::GetFont(LPCWSTR lpszId) const
{
	return m_loadData.GetFont(lpszId);
}

CUIView *CUILoadAttrs::GetView(LPCWSTR lpszId) const
{
	return m_loadData.GetView(lpszId);
}

}	// namespace

////////////////////////////////////////////////////////////////////////////////

class CUILoader
{
public:
	bool Load(CUIStream *pStream, CUIView *pView);

private:
	bool LoadView(tinyxml2::XMLElement *pElem, CUIView *pParent);
	bool AddChild(tinyxml2::XMLElement *pElem, CUIView *pParent);
	void InitFont(const IUILoadAttrs &attrs);

	CUILoadData m_loadData;
};

static bool IsNameClass(LPCSTR lpName, LPCVOID pView)
{
	if (lpName == NULL)
		return false;

#ifdef _DEBUG
	char szName[256];
	sprintf_s(szName, ".?AVCUI%s@@", lpName);
	return _stricmp(szName, (*(char ****)pView)[-1][3] + 8) == 0;
#else
	return true;
#endif
}

bool CUILoader::Load(CUIStream *pStream, CUIView *pView)
{
	// ¼ÓÔØ xml ×Ö·û´®
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError errId = doc.Parse(pStream->Data(), pStream->Size());

	if (errId != tinyxml2::XML_SUCCESS)
		return false;

	tinyxml2::XMLElement *pElem = doc.RootElement();
	if (pElem == NULL || !IsNameClass(pElem->Value(), pView))
	{
		ATLASSERT(0);
		return false;
	}

	// ½âÎö xml
	if (!LoadView(pElem, pView))
		return false;

	pView->OnLoaded(CUILoadAttrs(m_loadData, pElem));
	return true;
}

bool CUILoader::LoadView(tinyxml2::XMLElement *pElem, CUIView *pParent)
{
	for (pElem = pElem->FirstChildElement(); pElem; pElem = pElem->NextSiblingElement())
	{
		if (!AddChild(pElem, pParent))
			return false;
	}

	return true;
}

bool CUILoader::AddChild(tinyxml2::XMLElement *pElem, CUIView *pParent)
{
	LPCSTR lpName = pElem->Value();
	CUILoadAttrs attrs(m_loadData, pElem);

	if (_stricmp(lpName, "Font") == 0)
	{
		InitFont(attrs);
		return true;
	}

	CUIView *pView = NULL;
	LPCWSTR lpFileName = attrs.GetStr(L"image");

	if (_stricmp(lpName, "View") == 0)
	{
		pView = pParent->AddView();
	}
	else if (_stricmp(lpName, "Button") == 0)
	{
		pView = pParent->AddButton(lpFileName);
	}
	else if (_stricmp(lpName, "ButtonEx") == 0)
	{
		pView = pParent->AddButtonEx(lpFileName);
	}
	else if (_stricmp(lpName, "MenuButton") == 0)
	{
		pView = pParent->AddMenuButton(lpFileName);
	}
	else if (_stricmp(lpName, "CheckButton") == 0)
	{
		pView = pParent->AddCheckButton(lpFileName);
	}
	else if (_stricmp(lpName, "RadioButton") == 0)
	{
		pView = pParent->AddRadioButton(lpFileName);
	}
	else if (_stricmp(lpName, "ComboButton") == 0)
	{
		pView = pParent->AddComboButton();
	}
	else if (_stricmp(lpName, "StateButton") == 0)
	{
		pView = pParent->AddStateButton();
	}
	else if (_stricmp(lpName, "CheckBox") == 0)
	{
		pView = pParent->AddCheckBox(lpFileName);
	}
	else if (_stricmp(lpName, "RadioBox") == 0)
	{
		pView = pParent->AddRadioBox(lpFileName);
	}
	else if (_stricmp(lpName, "Animation") == 0)
	{
		pView = pParent->AddAnimation(lpFileName);
	}
	else if (_stricmp(lpName, "Edit") == 0)
	{
		pView = pParent->AddEdit();
	}
	else if (_stricmp(lpName, "Gif") == 0)
	{
		pView = pParent->AddGif(lpFileName);
	}
	else if (_stricmp(lpName, "Image") == 0)
	{
		pView = pParent->AddImage(lpFileName);
	}
	else if (_stricmp(lpName, "Label") == 0)
	{
		pView = pParent->AddLabel();
	}
	else if (_stricmp(lpName, "Line") == 0)
	{
		pView = pParent->AddLine();
	}
	else if (_stricmp(lpName, "Notice") == 0)
	{
		pView = pParent->AddNotice();
	}
	else if (_stricmp(lpName, "Progress") == 0)
	{
		pView = pParent->AddProgress(lpFileName);
	}
	else if (_stricmp(lpName, "PageView") == 0)
	{
		pView = pParent->AddPageView(lpFileName);
	}
	else if (_stricmp(lpName, "ScrollView") == 0)
	{
		pView = pParent->AddScrollView();
	}
	else if (_stricmp(lpName, "Slider") == 0)
	{
		pView = pParent->AddSlider(lpFileName, attrs.GetStr(L"imageBg"));
	}
	else if (_stricmp(lpName, "ToolBar") == 0)
	{
		pView = pParent->AddToolBar(lpFileName);
	}
	else if (_stricmp(lpName, "VScroll") == 0)
	{
		pView = pParent->AddVScroll(lpFileName, attrs.GetStr(L"imageBg"));
	}
	else if (_stricmp(lpName, "WebTabBar") == 0)
	{
		pView = pParent->AddWebTabBar(lpFileName);
	}
	else
	{
		if ((pView = pParent->GetRootView()->OnCustomUI(CA2W(lpName, CP_UTF8), pParent)) == NULL)
		{
			ATLASSERT(0);
			return false;
		}

		pParent->AddChild(pView);
	}

	if (LPCWSTR lpszId = attrs.GetStr(L"id"))
		m_loadData.SetView(lpszId, pView);

	if (!LoadView(pElem, pView))
		return false;

	pView->OnLoaded(attrs);
	return true;
}

void CUILoader::InitFont(const IUILoadAttrs &attrs)
{
	LPCWSTR lpszId = attrs.GetStr(L"id");

	if (lpszId && *lpszId)
	{
		int nWeight;
		if (attrs.GetInt(L"weight", &nWeight) && (UINT)nWeight <= FW_HEAVY / 100)
			nWeight *= 100;
		else if (attrs.GetInt(L"bold"))
			nWeight = FW_BOLD;
		else
			nWeight = FW_NORMAL;

		HFONT hFont = GetFont(attrs.GetStr(L"name"), nWeight, attrs.GetInt(L"italic"), attrs.GetInt(L"underline"));
		ATLASSERT(hFont);
		m_loadData.SetFont(lpszId, hFont);
	}
}

bool UILib::LoadFromXml(LPCWSTR lpXmlName, CUIView *pView)
{
	bool bRet = false;

	if (CUIStream *pStream = GetStream(lpXmlName))
	{
		CUILoader loader;
		bRet = loader.Load(pStream, pView);
		pStream->Release();
	}

	return bRet;
}
