#pragma once

#include "UIControl.h"

// 单行输入框

class UILIB_API CUIEdit : public CUIControl
{
public:
	CUIEdit(CUIView *pParent);
	virtual ~CUIEdit();

	void SetFont(HFONT hFont);
	HFONT GetFont() const { return m_hFont; }
	void SetTextColor(COLORREF color);
	void SetText(LPCWSTR lpText);
	LPCWSTR GetText() const { return m_strText.c_str(); }
	void Insert(LPWSTR lpText);
	void SetSel(int iStart, int iEnd);
	void SetFocus();
	void SetTabStop(int nTabStop) { m_nTabStop = nTabStop; }
	void NextTabStop(bool bShift);
	void SetPasswordChar(wchar_t chPassword) { m_chPassword = chPassword; }
	void SetPassword(bool bPassword) { m_bPassword = bPassword; }
	void SetReadonly(bool bReadonly) { m_bReadonly = bReadonly; }
	void SetImeEnabled(bool bEnable) { m_bImeEnable = bEnable; }
	bool IsImeEnabled() const { return m_bImeEnable && !m_bPassword && !m_bReadonly; }
	void SetMaxBytes(WORD nMaxLen) { m_nMaxLen = nMaxLen | 0x10000; }
	void SetMaxChars(WORD nMaxLen) { m_nMaxLen = nMaxLen; }
	void SetUIMenu(CUIMenu *pUIMenu);
	void BindFilter(function<void(LPWSTR)> &&fnFilter) { m_fnFilter = std::move(fnFilter); }

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual int  IsPaint() const override { return 2; }
	virtual void OnPaint(CUIDC &dc) const override;
	virtual void OnEnable(bool bEnable) override;
	virtual void OnVisible(bool bVisible) override;
	virtual void OnLButtonDown(CPoint point) override;
	virtual void OnLButtonUp(CPoint point) override;
	virtual void OnRButtonDown(CPoint point) override;
	virtual void OnRButtonUp(CPoint point) override;
	virtual void OnSetFocus() override;
	virtual void OnKillFocus() override;
	virtual void OnKeyDown(UINT nChar);
	virtual void OnKeyChar(UINT nChar);
	virtual void OnTextChange() {}
	virtual void CalcCaretPos();
	virtual void CheckMaxText(LPCWSTR lpExist, LPWSTR lpText) const;
	void OnMouseMove(CPoint point);
	void CalcFontHeight();
	void CalcClickPos(bool bShift, CPoint point);
	int  CalcPosLeft(int iPos) const;
	void CalcSelRect(int iStart, int iEnd, LPRECT lpRect) const;
	void InvalidateSel(int iStart, int iEnd);
	void OnKeyMove(int nMove);
	void DelStartEnd(int iStart, int iEnd, bool bChange = true);
	void OnUndo();
	void OnCut();
	void OnCopy();
	void OnPaste();
	void OnSelAll();

	wstring  m_strText;
	HFONT    m_hFont;
	COLORREF m_color;
	int      m_nTabStop;	// 按 Tab 键切换的顺序
	int      m_chPassword;
	bool     m_bPassword;
	bool     m_bReadonly;
	bool     m_bImeEnable;
	bool     m_bFocus;
	UINT     m_nMaxLen;		// 最大字符数(W)或字节数(A)
	int      m_nHeight;
	int      m_iStart;
	int      m_iEnd;
	int      m_nLeft;		// 左边界向左偏移量
	CUIMenu *m_pUIMenu;
	function<void(LPWSTR)> m_fnFilter;

	struct
	{
		wstring m_strDiff;
		int     m_nPos;
		int     m_nLen;

		void SetPosLen(int nPos)
		{
			m_nPos = nPos;
			m_nLen = 0;
		}
	} m_undo;
};
