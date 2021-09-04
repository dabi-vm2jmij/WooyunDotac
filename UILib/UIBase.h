#pragma once

class IUILoadAttrs;

class UILIB_API CUIBase
{
	friend class CUIView;
	friend class CUIRootView;
	friend class CUILoader;
public:
	CUIBase(CUIView *pParent);
	virtual ~CUIBase();

	void SetLeft(int nLeft, bool bClip = false);
	void SetRight(int nRight, bool bClip = false);
	void SetTop(int nTop, bool bClip = false);
	void SetBottom(int nBottom, bool bClip = false);
	void SetWidth(int nWidth);
	void SetHeight(int nHeight);
	void SetSize(CSize size);
	void GetSize(LPSIZE lpSize) const;
	void GetClientRect(LPRECT lpRect) const;
	void GetWindowRect(LPRECT lpRect) const;
	void SetDbgColor(COLORREF color) { m_dbgColor = color; }
	void SetEnabled(bool bEnabled);
	void SetVisible(bool bVisible);
	bool IsEnabled() const { return m_bEnabled; }
	bool IsVisible() const { return m_bVisible; }
	bool IsRealEnabled() const;
	bool IsRealVisible() const;
	bool IsChild(const CUIBase *pItem) const;
	bool IsControl() const { return m_bControl; }
	bool IsMouseEnter() const { return m_ppEnter != NULL; }
	CUIView *GetParent() const;
	CUIRootView *GetRootView() const;

protected:
	struct UIHitTest
	{
		CPoint point;
		UINT   nCount;

		struct
		{
			CUIBase *pItem;
			bool     bEnabled;
		} items[64];

		UIHitTest() : nCount(0) {}

		void Add(CUIBase *pItem)
		{
			if (nCount < _countof(items))
				items[nCount++] = { pItem, pItem->IsEnabled() };
		}

		auto begin() const -> decltype(items + 0)
		{
			return items;
		}

		auto end() const -> decltype(items + 0)
		{
			return items + nCount;
		}
	};

	virtual void OnLoaded(const IUILoadAttrs &attrs);
	virtual bool OnHitTest(UIHitTest &hitTest) { return false; }
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }
	virtual void OnPaint(CUIDC &dc) const;
	virtual void OnEnabled(bool bEnabled) {}
	virtual void OnRectChanged(LPCRECT lpOldRect, LPRECT lpClipRect) {}
	virtual void IsNeedLayout(LPRECT lpClipRect) {}
	virtual void CalcRect(LPRECT lpRect, LPRECT lpClipRect);
	virtual void OnMouseEnter() {}
	virtual void OnMouseLeave() {}
	virtual bool DoMouseLeave(bool bForce);
	void DoPaint(CUIDC &dc) const;

	CUIView  *m_pParent;
	CRect     m_offset;		// 对齐和偏移，左右互斥、上下互斥
	CSize     m_size;		// 宽或高小于零为自适应
	CRect     m_rect;		// 完整区域，包括不可见部分
	COLORREF  m_dbgColor;
	bool      m_bEnabled;
	bool      m_bVisible;
	bool      m_bControl;	// 是否 CUIControl
	bool      m_bKeepEnter;	// 保持 MouseEnter 状态

private:
	CRect     m_rcReal;		// 实际可见区域
	CUIBase **m_ppEnter;	// 参考 CUIRootView::DoMouseEnter

	CUIBase(const CUIBase &) = delete;
	CUIBase &operator=(const CUIBase &) = delete;
};

class UILIB_API CUIBlank : public CUIBase
{
public:
	CUIBlank(CUIView *pParent) : CUIBase(pParent) {}
};
