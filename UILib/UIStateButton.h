#pragma once

#include "UIControl.h"

// ״̬��ť�������ť��ϣ���ǰֻ��ʾ1����������л����¸�������ϵͳ����󻯡���ԭ��ť

class UILIB_API CUIStateButton : public CUIControl
{
public:
	CUIStateButton(CUIView *pParent);
	virtual ~CUIStateButton();

	void BindClick(function<void(int)> &&fnOnClick) { m_fnOnClick = std::move(fnOnClick); }
	void EndAddChild();
	void SetState(UINT nState);
	UINT GetState() const { return m_nState; }
	virtual HCURSOR GetCursor() const override;
	virtual LPCWSTR GetToolTip() const override;

protected:
	virtual void OnLoaded(const IUIXmlAttrs &attrs) override;
	virtual bool OnHitTest(UIHitTest &hitTest) override;
	virtual void OnLButtonDown(CPoint point) override;
	virtual void OnLButtonUp(CPoint point) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;

	UINT m_nState;
	function<void(int)> m_fnOnClick;
};
