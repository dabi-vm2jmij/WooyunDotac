#pragma once

#include "UIResource.h"
#include "UIFontMgr.h"
#include "UIToolTip.h"

class CUILibApp
{
public:
	CUILibApp();

	UINT GetLayoutMsg() const { return m_nLayoutMsg; }
	CUIResource &GetResource() { return m_resource; }
	CUIFontMgr &GetFontMgr() { return m_fontMgr; }
	void ShowTip(HWND hParent, LPCWSTR lpText);

private:
	UINT        m_nLayoutMsg;
	CUIResource m_resource;
	CUIFontMgr  m_fontMgr;
	CUIToolTip  m_toolTip;
};

extern CUILibApp g_theApp;
