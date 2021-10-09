#pragma once

class IUIXmlAttrs;

class UILIB_API CUIBase
{
	friend class CUIView;
	friend class CUIRootView;
	friend class CUILoader;
public:
	CUIBase(CUIView *pParent);
	virtual ~CUIBase();

	void SetId(const wstring &strId) { m_strId = strId; }
	void SetId(wstring &&strId) { m_strId = std::move(strId); }
	LPCWSTR GetId() const { return m_strId.c_str(); }
	void SetLeft(int nLeft, bool bClip = false);
	void SetRight(int nRight, bool bClip = false);
	void SetTop(int nTop, bool bClip = false);
	void SetBottom(int nBottom, bool bClip = false);
	void SetWidth(int nWidth);
	void SetHeight(int nHeight);
	void SetSize(CSize size);
	CSize GetSize() const { return m_size; }
	CRect GetClientRect() const { return m_rect; }
	CRect GetWindowRect() const;
	void InvalidateRect(LPCRECT lpRect);
	void SetBgColor(COLORREF color) { m_colorBg = color; }
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

	virtual void OnLoaded(const IUIXmlAttrs &attrs);
	virtual bool OnHitTest(UIHitTest &hitTest) { return false; }
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }
	virtual void OnPaint(CUIDC &dc) const {}
	virtual void OnEnabled(bool bEnabled) {}
	virtual void OnRectChanged(LPCRECT lpOldRect, LPRECT lpClipRect) {}
	virtual void OnNeedLayout(LPRECT lpClipRect) {}
	virtual void CalcRect(LPRECT lpRect, LPRECT lpClipRect);
	virtual void OnMouseEnter() {}
	virtual void OnMouseLeave() {}
	virtual bool DoMouseLeave(bool bForce);
	void DoPaint(CUIDC &dc) const;

	CUIView  *m_pParent;
	CRect     m_offset;		// 对齐和偏移，左右互斥、上下互斥
	CSize     m_size;		// 宽或高小于零为自适应
	CRect     m_rect;		// 完整区域，包括不可见部分
	wstring   m_strId;
	COLORREF  m_colorBg;
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
