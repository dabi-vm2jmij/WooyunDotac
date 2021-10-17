#pragma once

#include "UIButton.h"

// 浏览器标签

class UILIB_API CUIWebTab : public CUIButton
{
	friend class CUIWebTabBar;
public:
	CUIWebTab(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIWebTab();

	bool IsSelected() const { return m_bSelect; }
	void SetTabPage(CUIView *pTabPage) { m_pTabPage = pTabPage; }
	CUIView *GetTabPage() const { return m_pTabPage; }
	CUIWebTabBar *GetWebTabBar() const;

protected:
	virtual void OnLButtonDown(CPoint point) override;
	virtual void OnButtonState(ButtonState btnState) override;
	virtual void OnSelect(bool bSelect);

private:
	bool     m_bSelect;
	CUIView *m_pTabPage;	// tab 关联的页
};
