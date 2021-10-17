#pragma once

#include "UIControl.h"

// ��ϰ�ť�������ť���������롢�뿪
// ���Ϊ���������ͨ��ť���ұ���������ť��˵���ť

class UILIB_API CUIComboButton : public CUIControl
{
public:
	CUIComboButton(CUIView *pParent);
	virtual ~CUIComboButton();

	void EndAddChild();

protected:
	virtual void OnLoaded(const IUIXmlAttrs &attrs) override;
	virtual bool OnHitTest(UIHitTest &hitTest) override;
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void OnMouseEnter() override;
	virtual void OnMouseLeave() override;
	CUIControl *GetHitChild(CPoint point);

	bool m_bNoLeave;
};
