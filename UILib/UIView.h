#pragma once

class IUIXmlAttrs;
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
class CUIHotKey;
class CUIEdit;
class CUIGif;
class CUIImage;
class CUILabel;
class CUILine;
class CUIProgress;
class CUIPageCtrl;
class CUICarousel;
class CUIScrollView;
class CUISlider;
class CUIScrollBar;
class CUIToolBar;
class CUIWebTabBar;

class UILIB_API CUIView
{
	friend class CUILoader;
	friend class CUIRootView;
public:
	CUIView(CUIView *pParent);
	virtual ~CUIView();

	// 设置 id
	void SetId(const wstring &strId) { m_strId = strId; }
	void SetId(wstring &&strId) { m_strId = std::move(strId); }
	LPCWSTR GetId() const { return m_strId.c_str(); }

	// 设置上下左右
	void SetLeft(int nLeft, bool bAlign = false);
	void SetRight(int nRight, bool bAlign = false);
	void SetTop(int nTop, bool bAlign = false);
	void SetBottom(int nBottom, bool bAlign = false);
	CRect GetOffset() const { return m_offset; }

	// 设置宽高
	void SetWidth(int nWidth);
	void SetHeight(int nHeight);
	void SetSize(CSize size);
	CSize GetSize() const { return m_size; }

	// 设置区域
	void SetRect(LPCRECT lpRect, LPRECT lpClipRect);
	CRect GetRect() const { return m_rect; }
	CRect GetWindowRect() const;

	// 无效区域、布局
	void InvalidateRect(LPCRECT lpRect = NULL);
	void InvalidateLayout();

	// 设置背景色、图
	void DoPaint(CUIDC &dc) const;
	void SetBgColor(COLORREF color);
	void SetBgImage(const CImagex &imagex);

	// 设置可用、可见
	void SetEnabled(bool bEnable);
	void SetVisible(bool bVisible);
	bool IsEnabled() const { return m_bEnable; }
	bool IsVisible() const { return m_bVisible; }
	bool IsRealEnabled() const;
	bool IsRealVisible() const;
	bool IsFullVisible() const;
	bool IsChild(const CUIView *pItem) const;
	bool IsMouseEnter() const { return m_bMouseEnter; }

	// 获取父、顶层视图
	CUIView *GetParent() const;
	CUIRootView *GetRootView() const;

	// 添加、删除、查找子控件
	const vector<CUIView *> &GetChilds() const { return m_vecChilds; }
	void DeleteChild(CUIView *pItem);
	CUIView *Search(LPCWSTR lpszId, UINT nDepth = -1) const;
	CUIView *AddChild(CUIView *pItem);
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
	CUIHotKey *AddHotKey();
	CUIEdit *AddEdit();
	CUIGif *AddGif(LPCWSTR lpFileName);
	CUIImage *AddImage(LPCWSTR lpFileName);
	CUILabel *AddLabel();
	CUILine *AddLine();
	CUIProgress *AddProgress(LPCWSTR lpFileName);
	CUIPageCtrl *AddPageCtrl(LPCWSTR lpFileName);
	CUICarousel *AddCarousel();
	CUIScrollView *AddScrollView();
	CUISlider *AddSlider(LPCWSTR lpFileName, LPCWSTR lpBgFileName);
	CUIScrollBar *AddScrollBar(LPCWSTR lpFileName, LPCWSTR lpBgFileName);
	CUIToolBar *AddToolBar();
	CUIWebTabBar *AddWebTabBar(LPCWSTR lpFileName);

protected:
	struct UIHitTest;

	virtual CUIView *OnCustomUI(LPCWSTR lpName, CUIView *pParent);
	virtual void OnLoad(const IUIXmlAttrs &attrs);
	virtual void OnLoaded() {}
	virtual bool OnHitTest(UIHitTest &hitTest);
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual int  IsPaint() const { return m_bgColor != -1 || m_bgImagex; }
	virtual void OnPaint(CUIDC &dc) const;
	virtual void PaintChilds(CUIDC &dc) const;
	virtual void OnEnable(bool bEnable);
	virtual void OnVisible(bool bVisible) {}
	virtual void CalcRect(LPRECT lpRect, LPRECT lpClipRect);
	virtual void OnRectChange(LPCRECT lpOldRect, LPRECT lpClipRect);
	virtual void OnNeedLayout(LPRECT lpClipRect);
	virtual void RecalcLayout(LPRECT lpClipRect);
	virtual void OnMouseEnter() {}
	virtual void OnMouseLeave() {}
	virtual void OnChildMoving(CUIView *pItem, CPoint point);
	virtual void OnChildMoved(CUIView *pItem, CPoint point);
	static void CalcLeftRight(long &nLeft, long &nRight, long nOffsetLeft, long nOffsetRight, long nWidth);

	CUIView *m_pParent;
	CRect    m_offset;		// 上下左右的约束
	CSize    m_size;		// 宽、高的约束，< 0 为百分比
	CRect    m_rect;		// 完整区域，包括不可见部分
	CRect    m_rcReal;		// 实际可见区域
	wstring  m_strId;
	COLORREF m_bgColor;
	CImagex  m_bgImagex;	// 背景图，使用 Scale9Draw
	bool     m_bEnable;
	bool     m_bVisible;
	bool     m_bMouseEnter;
	bool     m_bNeedLayout;
	vector<CUIView *> m_vecChilds;

private:
	void MySetRect(CRect &rect, CRect &rcReal, LPRECT lpClipRect);
	void MyAddChild(CUIView *pItem);
	CUIView *MySearch(LPCWSTR lpszId, UINT nDepth) const;

	CUIView(const CUIView &) = delete;
	CUIView &operator=(const CUIView &) = delete;

	// 自动类型转换
	class CUIPtr
	{
		CUIView *m_ptr;
	public:
		CUIPtr(CUIView *ptr) : m_ptr(ptr) {}

		template<typename T>
		operator T *() const
		{
			T *ptr = dynamic_cast<T *>(m_ptr);
			ATLASSERT(ptr);
			return ptr;
		}
	};

public:
	CUIPtr SearchCast(LPCWSTR lpszId) const { return Search(lpszId); }
};

// 测试点击在哪些控件上
struct CUIView::UIHitTest
{
	CPoint point;
	UINT   nCount;

	struct
	{
		CUIView *pItem;
		bool     bEnable;
	} items[30];

	UIHitTest() : nCount(0) {}

	void Add(CUIView *pItem)
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
