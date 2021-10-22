#pragma once

#include "UIControl.h"

// ���������

class UILIB_API CUIEdit : public CUIControl
{
	friend class CUIRootView;
public:
	CUIEdit(CUIView *pParent);
	virtual ~CUIEdit();

	void SetFont(HFONT hFont, bool bRedraw = false);
	void SetTextColor(COLORREF color) { m_color = color; }
	void SetText(LPCWSTR lpText);
	LPCWSTR GetText() const { return m_strText.c_str(); }
	void SetSel(int iStart, int iEnd);
	void SetFocus();
	void SetPassword(bool bPassword);
	void SetReadonly(bool bReadonly) { m_bReadonly = bReadonly; }
	void SetMaxBytes(WORD nMaxLen) { m_nMaxLen = nMaxLen | 0x10000; }
	void SetMaxChars(WORD nMaxLen) { m_nMaxLen = nMaxLen; }
	void BindFilter(function<void(LPWSTR)> &&fnFilter) { m_fnFilter = std::move(fnFilter); }
	void BindKeyReturn(function<void()> &&fnOnReturn) { m_fnOnReturn = std::move(fnOnReturn); }

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void DoPaint(CUIDC &dc) const override;
	virtual void OnEnable(bool bEnable) override;
	virtual void OnLButtonDown(CPoint point) override;
	virtual void OnLButtonUp(CPoint point) override;
	virtual void OnRButtonDown(CPoint point) override;
	virtual void OnRButtonUp(CPoint point) override;
	virtual void OnSetFocus() override;
	virtual void OnKillFocus() override;
	virtual void OnInputLangChange(UINT nLocaleId);
	virtual void OnKeyDown(UINT nChar);
	virtual void OnKeyChar(UINT nChar);
	virtual void OnTextChange() {}
	virtual void CalcCaretPos(LPPOINT lpPoint);
	virtual void CheckMaxText(LPCWSTR lpExist, LPWSTR lpText) const;
	virtual CUIMenu *NewUIMenu() const { return new CUIMenu; }	// ���Լ̳к���д��ʵ���Զ����Ҽ��˵�
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
	void Insert(LPWSTR lpText);

	wstring  m_strText;
	bool     m_bPassword;
	bool     m_bReadonly;
	UINT     m_nMaxLen;	// ����ַ���(W)��HIWORD Ϊ�ֽ���(A)
	int      m_nHeight;
	int      m_iStart;
	int      m_iEnd;
	int      m_nLeft;	// ��߽�����ƫ����
	bool     m_bFocus;
	bool     m_bCaret;	// �Ƿ񻭹��
	HFONT    m_hFont;
	COLORREF m_color;
	CPoint   m_ptCaret;
	CUITimer m_uiTimer;
	function<void(LPWSTR)> m_fnFilter;
	function<void()> m_fnOnReturn;

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

private:
	void MyCreateCaret();
	void MyDestroyCaret();
	void MySetCaretPos();
	void OnUITimer();
	CPoint GetBasePoint() const;	// rootView ����� hWnd ������
};
