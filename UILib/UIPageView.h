#pragma once

#include "UIView.h"

// 分页 Radio，分页显示内容时，点击每个 Radio 切换到对应的页

class UILIB_API CUIPageView : public CUIView
{
public:
	CUIPageView(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIPageView();

	void OnNewRadio(function<CUIRadioBox *(int)> &&fnNewRadio) { m_fnNewRadio = std::move(fnNewRadio); }
	void OnChanged(function<void(int)> &&fnOnChanged) { m_fnOnChanged = std::move(fnOnChanged); }
	void SetSpace(int nSpace) { m_nSpace = nSpace; }
	void SetCount(UINT nCount);
	void SetIndex(UINT nIndex);
	UINT GetIndex() const;

protected:
	virtual void OnLoaded(const IUIXmlAttrs &attrs) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;

	wstring m_strImage;
	int     m_nSpace;	// 间距
	function<CUIRadioBox *(int)> m_fnNewRadio;
	function<void(int)> m_fnOnChanged;
};
