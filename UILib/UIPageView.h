#pragma once

#include "UIView.h"

// ��ҳ Radio����ҳ��ʾ����ʱ�����ÿ�� Radio �л�����Ӧ��ҳ

class UILIB_API CUIPageView : public CUIView
{
public:
	CUIPageView(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIPageView();

	void BindNewRadio(function<CUIRadioBox *(int)> &&fnNewRadio) { m_fnNewRadio = std::move(fnNewRadio); }
	void BindChange(function<void(int)> &&fnOnChange) { m_fnOnChange = std::move(fnOnChange); }
	void SetSpace(int nSpace) { m_nSpace = nSpace; }
	void SetCount(UINT nCount);
	void SetIndex(UINT nIndex);
	UINT GetIndex() const;

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;

	wstring m_strImage;
	int     m_nSpace;	// ���
	function<CUIRadioBox *(int)> m_fnNewRadio;
	function<void(int)> m_fnOnChange;
};
