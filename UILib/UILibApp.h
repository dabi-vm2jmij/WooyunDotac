#pragma once

#include "UIResource.h"
#include "UIFontMgr.h"

#define WM_NEEDLAYOUT	WM_USER + 1000

class CUILibApp : public CWindowImpl<CUILibApp>
{
public:
	DECLARE_WND_CLASS_EX(NULL, 0, -1)

	CUILibApp();
	~CUILibApp();

	CUIResource &GetResource() { return m_resource; }
	CUIFontMgr &GetFontMgr() { return m_fontMgr; }
	void RemoveLayout(CUIRootView *pRootView);
	void DelayLayout(CUIRootView *pRootView);

private:
	BEGIN_MSG_MAP(CUILibApp)
		MESSAGE_HANDLER(WM_NEEDLAYOUT, OnNeedLayout)
	END_MSG_MAP()

	LRESULT OnNeedLayout(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	CUIResource m_resource;
	CUIFontMgr  m_fontMgr;
	bool        m_bPostMsg;
	std::deque<CUIRootView *> m_deqRootViews;
};

extern CUILibApp g_theApp;
