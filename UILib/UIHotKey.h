#pragma once

class UILIB_API CUIHotKey : public CUIControl, public CUITextImpl
{
public:
	CUIHotKey(CUIView *pParent);

	void SetHotKey(UINT nHotKey);
	UINT GetHotKey() const;
	void SetFocus();

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void OnPaint(CUIDC &dc) const override;
	virtual void OnEnable(bool bEnable) override;
	virtual void OnVisible(bool bVisible) override;
	virtual void OnRectChange(LPCRECT lpOldRect, LPRECT lpClipRect) override;
	virtual void OnLButtonDown(CPoint point) override;
	virtual void OnSetFocus() override;
	virtual void OnKillFocus() override;
	virtual void OnTextSize(CSize size) override;

	union
	{
		struct
		{
			WORD m_nVKCode;
			WORD m_nModifiers;
		};

		UINT m_nHotKey;
	};

	bool m_bFocus;
};
