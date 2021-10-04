#pragma once

#include "UIButton.h"
#include "UITextImpl.h"

// �߼���ť���������ñ�����ͼ�ꡢ���֡�ֻ�������־��ǳ����Ӱ�ť

class UILIB_API CUIButtonEx : public CUIButton, public CUITextImpl
{
public:
	CUIButtonEx(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIButtonEx();

	void SetIconImage(LPCWSTR lpFileName, int nIconLeft);
	void SetTextColors(COLORREF *colors, int nCount);
	void SetTextLeft(int nTextLeft);
	void SetUnderline(bool bUnderline) { m_bUnderline = bUnderline; }

protected:
	virtual void OnLoaded(const IUIXmlAttrs &attrs) override;
	virtual void MyPaint(CUIDC &dc) const override;
	virtual void OnMouseEnter() override;
	virtual void OnMouseLeave() override;
	virtual void OnTextSize(CSize size) override;
	virtual void OnDrawState(UINT nState) override;

	CImagex  m_imgxIcons[4];
	COLORREF m_colors[4];
	int      m_nIconLeft;
	int      m_nTextLeft;
	bool     m_bUnderline;
};
