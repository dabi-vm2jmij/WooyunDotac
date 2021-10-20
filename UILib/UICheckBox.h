#pragma once

#include "UIControl.h"
#include "UITextImpl.h"

// ¸´Ñ¡¿ò

class UILIB_API CUICheckBox : public CUIControl, public CUITextImpl
{
public:
	CUICheckBox(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUICheckBox();

	void BindClick(function<void()> &&fnOnClick) { m_fnOnClick = std::move(fnOnClick); }
	void SetTextLeft(int nTextLeft);
	void SetCheck(bool bCheck);
	bool IsChecked() const { return m_bCheck; }

protected:
	virtual void OnLoaded(const IUIXmlAttrs &attrs) override;
	virtual void DoPaint(CUIDC &dc) const override;
	virtual void OnLButtonUp(CPoint point) override;
	virtual void OnTextSize(CSize size) override;
	virtual void OnChecked() {}

	CImagex m_imagexs[2];
	int     m_nTextLeft;
	bool    m_bCheck;
	function<void()> m_fnOnClick;
};
