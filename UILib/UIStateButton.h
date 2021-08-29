#pragma once

#include "UIView.h"

// ״̬��ť�������ť��ϣ���ǰֻ��ʾ1����������л����¸�������ϵͳ����󻯡���ԭ��ť

class UILIB_API CUIStateButton : public CUIView
{
public:
	CUIStateButton(CUIView *pParent);
	virtual ~CUIStateButton();

	void OnClick(std::function<void(int)> &&fnOnClick) { m_fnOnClick = std::move(fnOnClick); }
	void SetState(int nState);
	void EndAddChild();

protected:
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;
	virtual void OnChildClick();

	int  m_nState;
	std::function<void(int)> m_fnOnClick;
};
