#pragma once

#include "UIControl.h"

// 纵向滚动条

class UILIB_API CUIVScroll : public CUIControl
{
public:
	CUIVScroll(CUIView *pParent, LPCWSTR lpFileName, LPCWSTR lpFileNameBg);
	virtual ~CUIVScroll();

	void OnChanged(function<void(int)> &&fnOnChanged) { m_fnOnChanged = std::move(fnOnChanged); }
	void SetWheelRate(int nRate) { m_nWheelRate = nRate; }
	void SetRange(int nMinPos, int nMaxPos);	// nMinPos : nMaxPos == 滑块高度 : 滚动条高度
	void SetCurPos(int nCurPos);
	int  GetCurPos() const { return (int)(m_fCurPos + 0.5); }

protected:
	virtual void OnLoaded(const IUIXmlAttrs &attrs) override;
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void OnRectChanged(LPCRECT lpOldRect, LPRECT lpClipRect) override;
	virtual void OnLButtonDown(CPoint point) override;
	virtual void OnChildMoving(CUIControl *, CPoint point) override;
	void ResetOffset(int nOffset, bool bSetPos);

	CUIButton *m_pButton;
	int        m_nWheelRate;	// 滚轮滚动一次的百分比
	int        m_nMinPos;
	int        m_nMaxPos;
	double     m_fCurPos;
	function<void(int)> m_fnOnChanged;
};
