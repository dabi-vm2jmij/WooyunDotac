#pragma once

#include "UIView.h"

// ���������ؼ������������ӣ���ʾ���˵��۵��������๤�ߡ�

class UILIB_API CUIToolBar : public CUIView
{
public:
	CUIToolBar(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIToolBar();

	void SetMoreBg(LPCWSTR lpFileName);
	void CloseMoreWnd();

protected:
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;
	int  GetMoreIndex() const;
	void OnMoreBtn();

	wstring  m_strImageBg;
	HWND     m_hMoreWnd;
	std::vector<CUIBase *> m_vecMoreItems;
};
