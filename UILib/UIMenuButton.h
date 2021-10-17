#pragma once

#include "UIButton.h"

// �˵���ť����1�ε�����£���2�ε������

class UILIB_API CUIMenuButton : public CUIButton
{
public:
	CUIMenuButton(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIMenuButton();

	void BindGetUIMenu(function<CUIMenu *()> &&fnGetUIMenu) { m_fnGetUIMenu = std::move(fnGetUIMenu); }

protected:
	virtual void OnMouseLeave() override;
	virtual void OnLButtonDown(CPoint point) override;
	virtual void GetPopupPos(LPRECT lpRect);

	bool m_bNoLeave;
	function<CUIMenu *()> m_fnGetUIMenu;
};
