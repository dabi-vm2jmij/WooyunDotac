#pragma once

#include "UIView.h"

class IUIWindow
{
public:
	virtual CUIView *OnCustomUI(LPCWSTR lpName, CUIView *pParent) { return NULL; }
	virtual void OnLoadedUI(const IUIXmlAttrs &attrs) {}
	virtual void OnDrawBg(CUIDC &dc, LPCRECT lpRect) const {}
	virtual HWND GetHwnd() const = 0;
};

class UILIB_API CUIRootView : public CUIView
{
	friend class CUIEdit;
public:
	CUIRootView(IUIWindow *pOwner);
	virtual ~CUIRootView();

	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult);
	HWND GetHwnd() const;
	void SetWndAlpha(BYTE nWndAlpha);
	void DoPaint(HDC hDC, LPCRECT lpClipRect) const;
	void RaiseMouseMove();
	void DoMouseEnter(CUIBase *pItem);
	void SetFocus(CUIControl *pCtrl);
	virtual void SetCapture(CUIControl *pCtrl);
	virtual void InvalidateRect(LPCRECT lpRect);
	virtual BOOL ClientToScreen(LPPOINT lpPoint);
	virtual BOOL ScreenToClient(LPPOINT lpPoint);
	CUIView *OnCustomUI(LPCWSTR lpName, CUIView *pParent);
	CUIControl *GetCapture() const { return m_pCapture; }

protected:
	virtual void ShowToolTip(LPCWSTR lpTipText);
	virtual void OnLoaded(const IUIXmlAttrs &attrs) override;
	bool OnNcHitTest(CPoint point);
	void OnSize(CSize size);
	void OnPaint();
	void OnPaintLayered(HDC hDC);
	void OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void CheckMouseLeave(const UIHitTest &hitTest);
	void EnableImm(bool bEnabled);
	void AddTabsEdit(CUIEdit *pEdit);
	void DelTabsEdit(CUIEdit *pEdit);
	void NextTabsEdit();

	IUIWindow  *m_pOwner;
	CImage      m_imageWnd;		// ͸�����ڻ���
	CRect       m_rectClip;		// ��ǰ��Ч����
	BYTE        m_nWndAlpha;	// ͸���ȣ�0~255
	bool        m_bLayered;
	bool        m_bMouseEnter;
	HIMC        m_hImc;
	HCURSOR     m_hCursor;
	HWND        m_hToolTip;
	wstring     m_strTipText;
	CUIControl *m_pCapture;
	CUIControl *m_pCurFocus;
	vector<CUIEdit *> m_vecEdits;
	vector<CUIBase *> m_vecEnterItems;

private:
	// �Զ�����ת��
	class CUICast
	{
	public:
		CUICast(CUIView *ptr) : m_ptr(ptr) {}

		template<typename T>
		operator T *() const
		{
			T *ptr = dynamic_cast<T *>(m_ptr);
			ATLASSERT(ptr);
			return ptr;
		}

	private:
		CUIView *m_ptr;
	};

public:
	CUICast SearchCast(LPCWSTR lpszId) const
	{
		return Search(lpszId);
	}
};
