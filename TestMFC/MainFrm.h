// MainFrm.h : CMainFrame 类的接口
//

#pragma once

#include "UIWindow.h"

class CMainFrame : public CUIWindow
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	virtual ~CMainFrame();

	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnLoadUI(const IUIXmlAttrs &attrs) override;
	virtual void DoPaint(CUIDC &dc) const override;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowAbout();
	afx_msg void OnTestFunc();
	afx_msg LRESULT OnProgress(WPARAM wParam, LPARAM lParam);
	void DoSetup();
	void ShowBottom(bool bShow);

	CImagex      m_bgImagex;
	CImagex      m_bgImagex2;
	CUIView     *m_pViews[3];
	CUIProgress *m_pProgress;
	CUIMenu      m_uiMenu;
};
