#pragma once

#include "Resource.h"
#include "UIMenu2.h"

class CMainFrame : public CUIWindow
{
public:
	CMainFrame();
	~CMainFrame();

	void OnFinalMessage(HWND hWnd) override;

	BEGIN_MSG_MAP(CMainFrame)
		CHAIN_MSG_MAP(CUIWindow)
		COMMAND_ID_HANDLER(ID_TEST_ABOUT, OnShowAbout)
		COMMAND_ID_HANDLER(ID_TEST_TEST, OnTestFunc)
		MESSAGE_HANDLER(WM_USER + 1000, OnThreadResult)
	END_MSG_MAP()

private:
	void OnLoadedUI(const IUILoadAttrs &loaded) override;
	int  OnCreate(LPCREATESTRUCT lpCreateStruct) override;
	void OnClose() override;
	BOOL OnNcActivate(BOOL bActive) override { return TRUE; }
	void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpNCSP) override {}
	LRESULT OnShowAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
	LRESULT OnTestFunc(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
	LRESULT OnThreadResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	void OnDrawBg(CUIDC &dc, LPCRECT lpRect) override;
	void ShowBottom(bool bShow);
	void DoSetup();
	static UINT WINAPI WorkThread(LPVOID pParam);

	CImagex      m_imagexBg;
	CImagex      m_imagexBg2;
	CUIView     *m_pViews[3];
	CUIProgress *m_pProgress;
	HANDLE       m_hThread;
	CUIMenu2     m_uiMenu;
};
