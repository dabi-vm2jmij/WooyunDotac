#pragma once
#include "UIButton.h"

// �˵���ť����1�ε�����£���2�ε������

class UILIB_API CUIMenuButton : public CUIButton
{
public:
	CUIMenuButton(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIMenuButton();

	void OnGetUIMenu(std::function<CUIMenu *()> &&fnGetUIMenu) { m_fnGetUIMenu = std::move(fnGetUIMenu); }

protected:
	virtual void OnLButtonDown(CPoint point) override;
	virtual void GetPopupPos(LPRECT lpRect);

	std::function<CUIMenu *()> m_fnGetUIMenu;
};
