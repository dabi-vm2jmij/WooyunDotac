#pragma once

#include "UIView.h"

// 分页器控件，每个圆点可点击切换

class UILIB_API CUIPageCtrl : public CUIView
{
public:
	CUIPageCtrl(CUIView *pParent, LPCWSTR lpFileName);

	void BindChange(function<void(int)> &&fnOnChange) { m_fnOnChange = std::move(fnOnChange); }
	void SetSpacing(int nSpacing) { m_nSpacing = nSpacing; }
	void SetCount(UINT nCount);
	void SetIndex(UINT nIndex);
	UINT GetIndex() const;

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;

	wstring m_strTabImage;
	int     m_nSpacing;	// 间距
	function<void(int)> m_fnOnChange;
};
