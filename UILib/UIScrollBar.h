#pragma once

#include "UIControl.h"

// 滚动条控件

class UILIB_API CUIScrollBar : public CUIControl
{
public:
	CUIScrollBar(CUIView *pParent, LPCWSTR lpFileName, LPCWSTR lpBgFileName);
	virtual ~CUIScrollBar();

	bool IsVertical() const { return m_bVertical; }
	void BindChange(function<void()> &&fnOnChange) { m_fnOnChange = std::move(fnOnChange); }
	void SetWheelSize(int nSize) { m_nWheelSize = nSize; }
	void SetRange(int nMinPos, int nMaxPos);	// nMinPos : nMaxPos == 滑块高度 : 滚动条高度
	void SetCurPos(int nCurPos);
	int  GetCurPos() const { return (int)(m_fCurPos + 0.5); }

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void OnRectChange(LPCRECT lpOldRect, LPRECT lpClipRect) override;
	virtual void OnChildMoving(CUIView *, CPoint point) override;
	virtual void OnChildMoved(CUIView *, CPoint point) override {}
	virtual void OnLButtonDown(CPoint point) override;
	void ResetOffset(int nOffset, bool bSetPos);

	bool       m_bVertical;
	CUIButton *m_pButton;
	int        m_nWheelSize;	// 滚轮滚一次的距离，< 0 为百分比
	int        m_nMinPos;
	int        m_nMaxPos;
	double     m_fCurPos;
	function<void()> m_fnOnChange;
};
