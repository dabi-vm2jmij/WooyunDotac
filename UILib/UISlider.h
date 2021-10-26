#pragma once

#include "UIControl.h"

// »¬¶¯¿Ø¼þ

class UILIB_API CUISlider : public CUIControl
{
public:
	CUISlider(CUIView *pParent, LPCWSTR lpFileName, LPCWSTR lpBgFileName);

	void BindChange(function<void()> &&fnOnChange) { m_fnOnChange = std::move(fnOnChange); }
	void SetMaxPos(int nMaxPos);
	void SetCurPos(int nCurPos);
	int  GetCurPos() const { return (int)(m_fCurPos + 0.5); }

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void OnPaint(CUIDC &dc) const override;
	virtual void OnRectChange(LPCRECT lpOldRect, LPRECT lpClipRect) override;
	virtual void OnChildMoving(CUIView *, CPoint point) override;
	virtual void OnChildMoved(CUIView *, CPoint point) override {}
	virtual void OnLButtonDown(CPoint point) override;
	void ResetOffset(int nOffset, bool bSetPos);

	CUIButton *m_pButton;
	int        m_nMaxPos;
	double     m_fCurPos;
	function<void()> m_fnOnChange;
};
