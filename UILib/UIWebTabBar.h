#pragma once

#include "UIWebTab.h"

// 浏览器标签栏

class UILIB_API CUIWebTabBar : public CUIView
{
public:
	CUIWebTabBar(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIWebTabBar();

	void BindSelect(function<void()> &&fnOnSelect) { m_fnOnSelect = std::move(fnOnSelect); }
	void SetTabWidth(int nWidth) { m_nTabWidth = nWidth; }
	void SetTabSpace(int nSpace) { m_nTabSpace = nSpace; }
	UINT GetCurSel() const;
	CUIWebTab *GetCurTab() const;
	CUIWebTab *AddWebTab();
	void DeleteTab(CUIWebTab *pWebTab);
	void SelectTab(CUIWebTab *pWebTab);
	void SelectNext();
	void SelectPrev();

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void CalcRect(LPRECT lpRect, LPRECT lpClipRect) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;
	virtual void OnChildMoving(CUIControl *, CPoint point) override;
	virtual void OnChildMoved(CUIControl *, CPoint point) override;

	CImagex m_imgxTabs[4];
	int     m_nTabWidth;	// tab 最大宽度
	int     m_nTabSpace;	// tab 间距
	function<void()> m_fnOnSelect;
};
