#pragma once

#include "UIControl.h"

// ��ϰ�ť�������ť���������롢�뿪
// ���Ϊ���������ͨ��ť���ұ���������ť��˵���ť

class UILIB_API CUIComboButton : public CUIView
{
	friend class CUIButton;
public:
	CUIComboButton(CUIView *pParent);
	virtual ~CUIComboButton();

	void EndAddChild();

protected:
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	virtual void OnChildEnter(CUIButton *pChild);
	virtual void OnChildLeave(CUIButton *pChild);

	bool m_bInNotify;
};
