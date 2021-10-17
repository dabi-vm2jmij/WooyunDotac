#include "stdafx.h"
#include "UILoader.h"
#include "UILibApp.h"
#include "tinyxml2.h"

class CUILoader
{
public:
	bool Load(CUIStream *pStream, CUIView *pView);
	bool Load(tinyxml2::XMLElement *pElem, CUIView *pView);
	HFONT GetFont(LPCWSTR lpIdOrName) const;

private:
	bool LoadView(tinyxml2::XMLElement *pElem, CUIView *pParent);
	bool AddChild(tinyxml2::XMLElement *pElem, CUIView *pParent);
	void InitFont(const IUIXmlAttrs &attrs);

	std::map<wstring, CUIFontMgr::FontKey> m_mapFonts;
};

namespace
{

class CUIXmlAttrs : public IUIXmlAttrs
{
public:
	CUIXmlAttrs(const CUILoader &loader, tinyxml2::XMLElement *pElem);

	virtual LPCWSTR GetStr(LPCWSTR lpKey) const override;
	virtual bool    GetInt(LPCWSTR lpKey, int *pnValue) const override;
	virtual HFONT   GetFont(LPCWSTR lpIdOrName) const override;

private:
	const CUILoader &m_loader;
	std::map<wstring, wstring> m_mapAttrs;
};

wstring StrLower(LPCWSTR lpStr)
{
	wstring strRet(lpStr);
	CharLowerBuffW((LPWSTR)strRet.c_str(), strRet.size());
	return strRet;
}

CUIXmlAttrs::CUIXmlAttrs(const CUILoader &loader, tinyxml2::XMLElement *pElem) : m_loader(loader)
{
	for (const tinyxml2::XMLAttribute *pAttr = pElem->FirstAttribute(); pAttr; pAttr = pAttr->Next())
	{
		m_mapAttrs.emplace(CharLowerW(CA2W(pAttr->Name(), CP_UTF8)), (LPCWSTR)CA2W(pAttr->Value(), CP_UTF8));
	}
}

LPCWSTR CUIXmlAttrs::GetStr(LPCWSTR lpKey) const
{
	auto it = m_mapAttrs.find(StrLower(lpKey));
	return it != m_mapAttrs.end() ? it->second.c_str() : NULL;
}

bool CUIXmlAttrs::GetInt(LPCWSTR lpKey, int *pnValue) const
{
	LPCWSTR lpStr = GetStr(lpKey);
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

HFONT CUIXmlAttrs::GetFont(LPCWSTR lpIdOrName) const
{
	return m_loader.GetFont(lpIdOrName);
}

bool IsNameClass(LPCSTR lpName, LPCVOID pView)
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

}	// namespace

////////////////////////////////////////////////////////////////////////////////

bool CUILoader::Load(CUIStream *pStream, CUIView *pView)
{
	// 加载 xml 字符串
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError errId = doc.Parse(pStream->Data(), pStream->Size());

	if (errId != tinyxml2::XML_SUCCESS)
	{
		ATLASSERT(0);
		return false;
	}

	tinyxml2::XMLElement *pElem = doc.RootElement();
	if (pElem == NULL || !IsNameClass(pElem->Value(), pView))
	{
		ATLASSERT(0);
		return false;
	}

	// 解析 xml
	return Load(pElem, pView);
}

bool CUILoader::Load(tinyxml2::XMLElement *pElem, CUIView *pView)
{
	if (!LoadView(pElem, pView))
		return false;

	pView->OnLoaded(CUIXmlAttrs(*this, pElem));
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
	CUIXmlAttrs attrs(*this, pElem);

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
	else if (pView = pParent->GetRootView()->OnCustomUI(CA2W(lpName, CP_UTF8), pParent))
	{
		pParent->AddChild(pView);
	}
	else
	{
		ATLASSERT(0);
		return false;
	}

	if (!LoadView(pElem, pView))
		return false;

	pView->OnLoaded(attrs);
	return true;
}

void CUILoader::InitFont(const IUIXmlAttrs &attrs)
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

		LPCWSTR lpszName = attrs.GetStr(L"name");
		BOOL bItalic = attrs.GetInt(L"italic"), bUnderline = attrs.GetInt(L"underline");
		ATLVERIFY(m_mapFonts.emplace(std::piecewise_construct, std::forward_as_tuple(StrLower(lpszId)), std::forward_as_tuple(lpszName, nWeight, bItalic, bUnderline)).second);
	}
}

HFONT CUILoader::GetFont(LPCWSTR lpIdOrName) const
{
	// 字体名
	if (wcschr(lpIdOrName, ':'))
		return GetCachedFont(lpIdOrName);

	// 字体 id
	auto it = m_mapFonts.find(StrLower(lpIdOrName));
	return it != m_mapFonts.end() ? g_theApp.GetFontMgr().GetCachedFont(it->second) : NULL;
}

namespace UILib
{

bool LoadFromXml(LPCWSTR lpXmlName, CUIView *pView)
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

}
