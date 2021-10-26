#pragma once

#include "UIView.h"

// ��ҳ���ؼ���ÿ��Բ��ɵ���л�

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
	int     m_nSpacing;	// ���
	function<void(int)> m_fnOnChange;
};
