#pragma once

#include "UIWebTab.h"

// 浏览器标签栏

class UILIB_API CUIWebTabBar : public CUIView
{
public:
	CUIWebTabBar(CUIView *pParent, LPCWSTR lpFileName);

	void BindSelect(function<void()> &&fnOnSelect) { m_fnOnSelect = std::move(fnOnSelect); }
	void SetTabWidth(int nWidth) { m_nTabWidth = nWidth; }
	void SetSpacing(int nSpacing) { m_nSpacing = nSpacing; }
	UINT GetCurSel() const;
	CUIWebTab *GetCurTab() const;
	CUIWebTab *AddWebTab();
	void DeleteTab(CUIWebTab *pWebTab);
	void SelectTab(CUIWebTab *pWebTab);
	void SelectNext();
	void SelectPrev();

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void PaintChilds(CUIDC &dc) const override;
	virtual void CalcRect(LPRECT lpRect, LPRECT lpClipRect) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;
	virtual void OnChildMoving(CUIView *, CPoint point) override;
	virtual void OnChildMoved(CUIView *, CPoint point) override;

	wstring m_strTabImage;
	int     m_nTabWidth;	// tab 最大宽度
	int     m_nSpacing;		// tab 间距
	function<void()> m_fnOnSelect;
};
