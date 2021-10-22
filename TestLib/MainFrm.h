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
		MESSAGE_HANDLER(WM_USER + 1000, OnProgress)
	END_MSG_MAP()

	virtual void OnFinalMessage(HWND hWnd) override;
	virtual void OnLoadUI(const IUIXmlAttrs &attrs) override;
	virtual void DoPaint(CUIDC &dc) const override;
	virtual void OnCreate() override;
	virtual void OnClose() override;
	LRESULT OnShowAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
	LRESULT OnTestFunc(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
	LRESULT OnProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	void DoSetup();
	void ShowBottom(bool bShow);
	void ShowDemo1();
	void ShowDemo2();

	CImagex      m_bgImagex;
	CImagex      m_bgImagex2;
	CUIView     *m_pViews[3];
	CUIProgress *m_pProgress;
	HANDLE       m_hThread;
	CUIMenu2     m_uiMenu;
};
