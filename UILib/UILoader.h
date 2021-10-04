#pragma once

class IUIXmlAttrs
{
public:
	virtual LPCWSTR GetStr(LPCWSTR lpKey) const = 0;
	virtual bool    GetInt(LPCWSTR lpKey, int *pnValue) const = 0;
	virtual HFONT   GetFont(LPCWSTR lpIdOrName) const = 0;

	int GetInt(LPCWSTR lpKey, int nDefault = 0) const
	{
		int nValue;
		return GetInt(lpKey, &nValue) ? nValue : nDefault;
	}
};

namespace UILib
{

UILIB_API bool LoadFromXml(LPCWSTR lpXmlName, CUIView *pView);

}
