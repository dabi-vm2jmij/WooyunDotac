#pragma once

class CDemoWnd1 : public CUIWindow
{
public:
	CDemoWnd1();

private:
	virtual void OnCreate() override;
	virtual void OnGetMinMaxInfo(MINMAXINFO *lpMMI) override;
	virtual void OnSize(UINT nType, CSize size);

	void InitUI();
	void AddWebTab();
	void DelWebTab(CUIWebTab *pWebTab);

	CUIWebTabBar *m_pWebTabBar;
	CUIView      *m_pTabView;
};
