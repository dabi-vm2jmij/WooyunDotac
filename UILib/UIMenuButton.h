#pragma once

#include "UIButton.h"

// �˵���ť����1�ε�����£���2�ε������

class UILIB_API CUIMenuButton : public CUIButton
{
public:
	CUIMenuButton(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIMenuButton();

	void BindPopup(function<void()> &&fnOnPopup) { m_fnOnPopup = std::move(fnOnPopup); }

protected:
	virtual void OnMouseLeave() override;
	virtual void OnLButtonDown(CPoint point) override;

	bool m_bNoLeave;
	function<void()> m_fnOnPopup;
};
