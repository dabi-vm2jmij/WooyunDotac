#pragma once

#include "UIButton.h"
#include "UITextImpl.h"

// 高级按钮，可以设置背景、图标、文字。只设置文字就是超链接按钮

class UILIB_API CUIButtonEx : public CUIButton, public CUITextImpl
{
public:
	CUIButtonEx(CUIView *pParent, LPCWSTR lpFileName);

	void SetIcon(const CImagex &imagex);
	void SetIcons(const CImagex imagexs[], int nCount);
	CImagex GetIcon() const { return m_iconxs[0]; }
	void SetIconLeft(int nIconLeft);
	void SetTextLeft(int nTextLeft);
	void SetTextColors(const COLORREF colors[], int nCount);
	void SetUnderline(bool bUnderline) { m_bUnderline = bUnderline; }

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void OnPaint(CUIDC &dc) const override;
	virtual void OnButtonState(ButtonState btnState) override;
	virtual void OnTextSize(CSize size) override;

	CImagex  m_iconxs[4];
	COLORREF m_colors[4];
	int      m_nIconLeft;	// icon 距离左边的偏移
	int      m_nTextLeft;	// text 距离左边的偏移
	bool     m_bUnderline;
};
