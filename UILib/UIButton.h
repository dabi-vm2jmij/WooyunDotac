#pragma once

#include "UIControl.h"

// ∆’Õ®∞¥≈•

class UILIB_API CUIButton : public CUIControl
{
public:
	CUIButton(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIButton();

	void OnClick(std::function<void()> &&fnOnClick) { m_fnOnClick = std::move(fnOnClick); }

protected:
	virtual void OnEnabled(bool bEnabled) override;
	virtual void OnMouseEnter() override;
	virtual void OnMouseLeave() override;
	virtual void OnLButtonDown(CPoint point) override;
	virtual void OnLButtonUp(CPoint point) override;
	virtual void OnDrawState(UINT nState);
	void ResetImages(const CImagex imagexs[], int nCount);
	void Fill4States(CImagex imagexs[], int nCount) const;

	CImagex m_imagexs[4];
	UINT    m_nDrawState;
	std::function<void()> m_fnOnClick;
};
