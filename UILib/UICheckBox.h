#pragma once

#include "UIControl.h"
#include "UITextImpl.h"

// ��ѡ��

class UILIB_API CUICheckBox : public CUIControl, public CUITextImpl
{
public:
	CUICheckBox(CUIView *pParent, LPCWSTR lpFileName);

	void BindClick(function<void()> &&fnOnClick) { m_fnOnClick = std::move(fnOnClick); }
	void SetSpacing(int nSpacing);
	void SetCheck(bool bCheck);
	bool IsChecked() const { return m_bCheck; }

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void OnPaint(CUIDC &dc) const override;
	virtual void OnLButtonUp(CPoint point) override;
	virtual void OnTextSize(CSize size) override;
	virtual void OnChecked() {}

	CImagex m_imagex;
	int     m_nSpacing;	// ͼ�ּ��
	bool    m_bCheck;
	function<void()> m_fnOnClick;
};
