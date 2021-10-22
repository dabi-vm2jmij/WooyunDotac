#pragma once

#include "UIControl.h"

// ∆’Õ®∞¥≈•

class UILIB_API CUIButton : public CUIControl
{
public:
	CUIButton(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIButton();

	enum ButtonState { Normal, Hover, Press, Disable };

	void BindClick(function<void()> &&fnOnClick) { m_fnOnClick = std::move(fnOnClick); }
	void SetImage(const CImagex &imagex);
	void SetImages(const CImagex imagexs[], int nCount);

protected:
	virtual void OnPaint(CUIDC &dc) const override;
	virtual void OnEnable(bool bEnable) override;
	virtual void OnMouseEnter() override;
	virtual void OnMouseLeave() override;
	virtual void OnLButtonDown(CPoint point) override;
	virtual void OnLButtonUp(CPoint point) override;
	virtual void OnButtonState(ButtonState btnState);

	CImagex     m_imagexs[4];
	ButtonState m_btnState;
	function<void()> m_fnOnClick;
};
