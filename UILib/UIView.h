#pragma once

#include "UIBase.h"

class CUIControl;
class CUIButton;
class CUIButtonEx;
class CUIMenuButton;
class CUICheckButton;
class CUIRadioButton;
class CUIComboButton;
class CUIStateButton;
class CUICheckBox;
class CUIRadioBox;
class CUIAnimation;
class CUIEdit;
class CUIGif;
class CUIImage;
class CUILabel;
class CUILine;
class CUINotice;
class CUIProgress;
class CUIPageView;
class CUIScrollView;
class CUISlider;
class CUIToolBar;
class CUIVScroll;
class CUIWebTabBar;

class UILIB_API CUIView : public CUIBase
{
public:
	CUIView(CUIView *pParent);
	virtual ~CUIView();

	void InvalidateRect(LPCRECT lpRect);
	void InvalidateLayout();
	void RemoveChild(CUIBase *pItem);
	UINT GetCount() const { return m_vecChilds.size(); }
	CUIBase *GetChild(UINT nIndex) const;
	CUIBase *AddChild(CUIBase *pItem);

	CUIView *AddView();
	CUIButton *AddButton(LPCWSTR lpFileName);
	CUIButtonEx *AddButtonEx(LPCWSTR lpFileName);
	CUIMenuButton *AddMenuButton(LPCWSTR lpFileName);
	CUICheckButton *AddCheckButton(LPCWSTR lpFileName);
	CUIRadioButton *AddRadioButton(LPCWSTR lpFileName);
	CUIComboButton *AddComboButton();
	CUIStateButton *AddStateButton();
	CUICheckBox *AddCheckBox(LPCWSTR lpFileName);
	CUIRadioBox *AddRadioBox(LPCWSTR lpFileName);
	CUIAnimation *AddAnimation(LPCWSTR lpFileName);
	CUIEdit *AddEdit();
	CUIGif *AddGif(LPCWSTR lpFileName);
	CUIImage *AddImage(LPCWSTR lpFileName);
	CUILabel *AddLabel();
	CUILine *AddLine();
	CUINotice *AddNotice();
	CUIProgress *AddProgress(LPCWSTR lpFileName);
	CUIPageView *AddPageView(LPCWSTR lpFileName);
	CUIScrollView *AddScrollView();
	CUISlider *AddSlider(LPCWSTR lpFileName, LPCWSTR lpFileNameBg);
	CUIToolBar *AddToolBar(LPCWSTR lpFileName);
	CUIVScroll *AddVScroll(LPCWSTR lpFileName, LPCWSTR lpFileNameBg);
	CUIWebTabBar *AddWebTabBar(LPCWSTR lpFileName);

	void OnRadioCheck(const CUIRadioBox *pItem);
	void OnRadioCheck(const CUIRadioButton *pItem);

protected:
	virtual bool OnHitTest(UIHitTest &hitTest) override;
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void OnPaint(CUIDC &dc) const override;
	virtual void OnEnabled(bool bEnabled) override;
	virtual void OnRectChanged(LPCRECT lpOldRect, LPRECT lpClipRect) override;
	virtual void IsNeedLayout(LPRECT lpClipRect) override;
	virtual void RecalcLayout(LPRECT lpClipRect);
	virtual void OnChildMoving(CUIControl *pCtrl, CPoint point) {}
	virtual void OnChildMoved(CUIControl *pCtrl, CPoint point) {}
	void PushBackChild(CUIBase *pItem);

	bool m_bNeedLayout;
	std::vector<CUIBase *> m_vecChilds;
};
