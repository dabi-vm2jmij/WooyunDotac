#pragma once

#include "Resource.h"
#include "UIMenu2.h"

class CMainFrame : public CUIWindow
{
public:
	CMainFrame();
	~CMainFrame();

private:
	BEGIN_MSG_MAP(CMainFrame)
		CHAIN_MSG_MAP(CUIWindow)
		COMMAND_ID_HANDLER(ID_TEST_ABOUT, OnShowAbout)
		COMMAND_ID_HANDLER(ID_TEST_TEST, OnTestFunc)
		MESSAGE_HANDLER(WM_USER + 1000, OnThreadResult)
	END_MSG_MAP()

	void OnFinalMessage(HWND hWnd) override;
	void OnLoadUI(const IUIXmlAttrs &attrs) override;
	int  OnCreate(LPCREATESTRUCT lpCreateStruct) override;
	void OnClose() override;
	LRESULT OnShowAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
	LRESULT OnTestFunc(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
	LRESULT OnThreadResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	void OnDrawBg(CUIDC &dc, LPCRECT lpRect) const override;
	void ShowBottom(bool bShow);
	void DoSetup();
	static UINT WINAPI WorkThread(LPVOID pParam);

	CImagex      m_bgImagex;
	CImagex      m_bgImagex2;
	CUIView     *m_pViews[3];
	CUIProgress *m_pProgress;
	HANDLE       m_hThread;
	CUIMenu2     m_uiMenu;
};
