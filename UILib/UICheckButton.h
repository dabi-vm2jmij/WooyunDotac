#pragma once

#include "UIButton.h"

// ��ѡ��ť���������ѡ�л���ȡ��ѡ��

class UILIB_API CUICheckButton : public CUIButton
{
public:
	CUICheckButton(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUICheckButton();

	void SetCheck(bool bCheck);
	bool IsChecked() const { return m_bCheck; }

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void OnLButtonUp(CPoint point) override;
	virtual void OnButtonState(ButtonState btnState) override;
	virtual void OnChecked() {}

	bool m_bCheck;
};
