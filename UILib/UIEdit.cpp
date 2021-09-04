#include "stdafx.h"
#include "UIEdit.h"

#pragma comment(lib, "imm32.lib")

#ifndef ID_EDIT_COPY
#define ID_EDIT_DELETE			0xE120
#define ID_EDIT_COPY			0xE122
#define ID_EDIT_CUT				0xE123
#define ID_EDIT_PASTE			0xE125
#define ID_EDIT_SELECT_ALL		0xE12A
#define ID_EDIT_UNDO			0xE12B
#endif

CUIEdit::CUIEdit(CUIView *pParent) : CUIControl(pParent), m_bPassword(false), m_bReadonly(false), m_nMaxLen(0), m_iStart(0), m_iEnd(0), m_nLeft(0)
	, m_bFocus(false), m_bCaret(false), m_hFont(GetDefaultFont()), m_color(0), m_uiTimer([this]{ OnUITimer(); })
{
	GetRootView()->AddTabsEdit(this);	// 加入到 CUIRootView 以支持 Tab 键
	CalcFontHeight();
	SetCursor(LoadCursor(NULL, IDC_IBEAM));

	m_undo.SetPosLen(-1);
}

CUIEdit::~CUIEdit()
{
	GetRootView()->DelTabsEdit(this);
}

void CUIEdit::SetFont(HFONT hFont, bool bRedraw)
{
	ATLASSERT(hFont);
	if (m_hFont == hFont || hFont == NULL)
		return;

	m_hFont = hFont;
	CalcFontHeight();

	if (bRedraw)
		InvalidateRect(NULL);
}

void CUIEdit::SetText(LPCWSTR lpText)
{
	int nSize = (wcslen(lpText) + 1) * 2;
	LPWSTR szText = (LPWSTR)alloca(nSize);
	CopyMemory(szText, lpText, nSize);

	CheckMaxText(L"", szText);

	InvalidateSel(0, -1);
	m_strText = szText;
	m_iStart = m_iEnd = m_nLeft = 0;
	InvalidateSel(0, -1);

	m_undo.SetPosLen(-1);
	m_undo.m_strDiff.clear();

	OnChanged();
	MySetCaretPos();
}

void CUIEdit::SetSel(int iStart, int iEnd)
{
	// 原选中区域
	CRect rect1;
	CalcSelRect(m_iStart, m_iEnd, rect1);

	// 重新选中
	m_iStart = min(m_strText.size(), (UINT)iStart);
	m_iEnd   = min(m_strText.size(), (UINT)iEnd);
	MySetCaretPos();

	if (!m_bFocus)
		return;

	// 现选中区域
	CRect rect2;
	CalcSelRect(m_iStart, m_iEnd, rect2);

	// 无效不重叠区域
	CRect rect;
	if (rect.IntersectRect(rect1, rect2))
	{
		std::swap(rect1.left, rect2.right);
		rect1.NormalizeRect();
		rect2.NormalizeRect();
	}

	InvalidateRect(rect1);
	InvalidateRect(rect2);
}

void CUIEdit::SetFocus()
{
	if (m_rect.IsRectEmpty())
	{
		ATLASSERT(0);
		return;
	}

	GetRootView()->SetFocus(this);
}

void CUIEdit::SetPassword(bool bPassword)
{
	if (m_bPassword != bPassword)
	{
		m_bPassword = bPassword;
		InvalidateRect(NULL);
	}
}

bool CUIEdit::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MOUSEMOVE:
		if (m_bLButtonDown)
		{
			OnMouseMove(lParam);
			return true;
		}
		break;

	case WM_LBUTTONDBLCLK:
		OnSelAll();
		return true;

	case WM_KEYDOWN:
		OnKeyDown(wParam);
		return true;

	case WM_CHAR:
		OnKeyChar(wParam);
		return true;

	case WM_IME_CHAR:
		OnKeyChar(wParam);
		return true;
	}

	return __super::OnMessage(uMsg, wParam, lParam);
}

void CUIEdit::MyPaint(CUIDC &dc) const
{
	// 密码显示星号
	LPCWSTR lpText = m_strText.c_str();
	if (lpText[0] && m_bPassword)
	{
		int    nSize  = m_strText.size();
		LPWSTR lpData = (LPWSTR)alloca((nSize + 1) * 2);
		lpText = lpData;

		for (int i = 0; i != nSize; i++)
			lpData[i] = '*';

		lpData[nSize] = 0;
	}

	CRect rect(m_rect);
	rect.top = (m_rect.top + m_rect.bottom - m_nHeight) / 2;
	rect.bottom = rect.top + m_nHeight;
	rect.left -= m_nLeft;

	dc.SelectFont(m_hFont);
	dc.SetTextColor(IsRealEnabled() ? m_color : RGB(110, 110, 110));
	DrawTextW(dc, lpText, -1, rect, DT_NOPREFIX | DT_SINGLELINE);

	if (m_iStart != m_iEnd && m_bFocus)	// 有焦点时
	{
		CalcSelRect(m_iStart, m_iEnd, rect);
		dc.FillSolidRect(rect, RGB(51, 153, 255));
		dc.SetTextColor(RGB(255, 255, 255));
		DrawTextW(dc, lpText + min(m_iStart, m_iEnd), abs(m_iStart - m_iEnd), rect, DT_NOPREFIX | DT_SINGLELINE);
	}

	if (m_bCaret)	// 自绘光标
	{
		int nOldRop = SetROP2(dc, R2_NOT);
		MoveToEx(dc, m_ptCaret.x, m_ptCaret.y, NULL);
		LineTo(dc, m_ptCaret.x, m_ptCaret.y + m_nHeight);
		SetROP2(dc, nOldRop);
	}

	if (dc.IsLayered())
		FillAlpha(dc, m_rect, 255);
}

void CUIEdit::OnEnabled(bool bEnabled)
{
	__super::OnEnabled(bEnabled);

	if (m_bFocus && !bEnabled)
		GetRootView()->SetFocus(NULL);

	InvalidateSel(0, -1);
}

void CUIEdit::OnRectChanged(LPCRECT lpOldRect, LPRECT lpClipRect)
{
	__super::OnRectChanged(lpOldRect, lpClipRect);

	if (m_bFocus && m_rect.IsRectEmpty())
		GetRootView()->SetFocus(NULL);
}

void CUIEdit::OnMouseMove(CPoint point)
{
	int iEnd = m_iEnd;
	CalcClickPos(true, point);
	InvalidateSel(iEnd, m_iEnd);
	MySetCaretPos();
}

void CUIEdit::OnLButtonDown(CPoint point)
{
	bool bShift = GetKeyState(VK_SHIFT) < 0;
	GetRootView()->SetCapture(this);

	if (m_bFocus)
	{
		if (!bShift)
			InvalidateSel(m_iStart, m_iEnd);

		int iEnd = m_iEnd;
		CalcClickPos(bShift, point);

		if (bShift)
			InvalidateSel(iEnd, m_iEnd);

		MySetCaretPos();
	}
	else
	{
		CalcClickPos(bShift, point);
		SetFocus();
	}
}

void CUIEdit::OnLButtonUp(CPoint point)
{
	GetRootView()->SetCapture(NULL);
}

void CUIEdit::OnRButtonDown(CPoint point)
{
	SetFocus();
}

void CUIEdit::OnRButtonUp(CPoint point)
{
	GetCursorPos(&point);

	CUIMenu *pUIMenu = NewUIMenu();
	pUIMenu->GetItem(ID_EDIT_UNDO)->m_strText = L"撤销(&U)";
	pUIMenu->GetItem();
	pUIMenu->GetItem(ID_EDIT_CUT)->m_strText = L"剪切(&T)";
	pUIMenu->GetItem(ID_EDIT_COPY)->m_strText = L"复制(&C)";
	pUIMenu->GetItem(ID_EDIT_PASTE)->m_strText = L"粘贴(&P)";
	pUIMenu->GetItem(ID_EDIT_DELETE)->m_strText = L"删除(&D)";
	pUIMenu->GetItem();
	pUIMenu->GetItem(ID_EDIT_SELECT_ALL)->m_strText = L"全选(&A)";

	// 撤销
	if (m_undo.m_nLen == 0 && m_undo.m_strDiff.empty())
		pUIMenu->GetItem(ID_EDIT_UNDO)->m_bEnabled = false;

	// 剪切、复制、删除
	if (m_iStart == m_iEnd)
	{
		pUIMenu->GetItem(ID_EDIT_CUT)->m_bEnabled = false;
		pUIMenu->GetItem(ID_EDIT_COPY)->m_bEnabled = false;
		pUIMenu->GetItem(ID_EDIT_DELETE)->m_bEnabled = false;
	}

	// 粘贴
	BOOL bPaste = FALSE;
	if (OpenClipboard(NULL))
	{
		bPaste = IsClipboardFormatAvailable(CF_UNICODETEXT);
		CloseClipboard();
	}

	if (!bPaste)
		pUIMenu->GetItem(ID_EDIT_PASTE)->m_bEnabled = false;

	// 全选
	if (abs(m_iStart - m_iEnd) == m_strText.size())
		pUIMenu->GetItem(ID_EDIT_SELECT_ALL)->m_bEnabled = false;

	// 只读
	if (m_bReadonly)
	{
		pUIMenu->GetItem(ID_EDIT_UNDO)->m_bEnabled = false;
		pUIMenu->GetItem(ID_EDIT_CUT)->m_bEnabled = false;
		pUIMenu->GetItem(ID_EDIT_PASTE)->m_bEnabled = false;
		pUIMenu->GetItem(ID_EDIT_DELETE)->m_bEnabled = false;
	}

	// 弹出菜单时光标暂停
	m_uiTimer.Kill();
	UINT nCmdId = pUIMenu->Popup(GetRootView()->GetHwnd(), point.x, point.y, MAXINT16, point.y, false);
	delete pUIMenu;
	m_uiTimer.Set(GetCaretBlinkTime());

	switch (nCmdId)
	{
	case ID_EDIT_UNDO:
		OnUndo();
		break;

	case ID_EDIT_CUT:
		OnCut();
		break;

	case ID_EDIT_COPY:
		OnCopy();
		break;

	case ID_EDIT_PASTE:
		OnPaste();
		break;

	case ID_EDIT_DELETE:
		DelStartEnd(m_iStart, m_iEnd);
		break;

	case ID_EDIT_SELECT_ALL:
		OnSelAll();
		break;
	}
}

void CUIEdit::OnSetFocus()
{
	if (m_bFocus)
		return;

	m_bFocus = true;
	InvalidateSel(m_iStart, m_iEnd);
	GetRootView()->EnableImm(!m_bPassword);	// 输入法
	MyCreateCaret();
}

void CUIEdit::OnKillFocus()
{
	if (!m_bFocus)
		return;

	m_bFocus = false;
	MyDestroyCaret();
	InvalidateSel(m_iStart, m_iEnd);
}

void CUIEdit::OnInputLangChange(UINT nLocaleId)
{
	HWND hWnd = GetRootView()->GetHwnd();
	HIMC hImc = ImmGetContext(hWnd);

	if (hImc)
	{
		LOGFONTW lf;
		GetObjectW(m_hFont, sizeof(lf), &lf);
		ImmSetCompositionFontW(hImc, &lf);

		COMPOSITIONFORM compForm = { CFS_POINT, m_ptCaret };
		ImmSetCompositionWindow(hImc, &compForm);
		ImmReleaseContext(hWnd, hImc);
	}
}

void CUIEdit::CalcFontHeight()
{
	TEXTMETRIC tm;
	GetTextMetrics(CUIComDC(m_hFont), &tm);
	m_nHeight = tm.tmHeight;
}

void CUIEdit::CalcClickPos(bool bShift, CPoint point)
{
	if ((m_iEnd = m_strText.size()) == 0)
	{
		m_iStart = 0;
		return;
	}

	CUIComDC dc(m_hFont);

	// 二分法查找
	int nWidth = point.x - m_rect.left + m_nLeft;
	int nWidthL = 0, nCountL = 0, nCountR = m_iEnd;

	for (;;)
	{
		int nCountM = (nCountL + nCountR + 1) / 2;

		CRect rect;
		DrawTextW(dc, m_strText.c_str() + nCountL, nCountM - nCountL, rect, DT_NOPREFIX | DT_SINGLELINE | DT_CALCRECT);
		int nWidthM = nWidthL + rect.Width();

		if (nCountM == nCountR)
		{
			m_iEnd = nWidth < (nWidthL + nWidthM) / 2 ? nCountL : nCountR;
			break;
		}

		if (nWidthM == nWidth)
		{
			m_iEnd = nCountM;
			break;
		}

		if (nWidthM < nWidth)
		{
			nCountL = nCountM;
			nWidthL = nWidthM;
		}
		else
			nCountR = nCountM;
	}

	if (!bShift)
		m_iStart = m_iEnd;
}

int CUIEdit::CalcPosLeft(int iPos) const
{
	if (iPos == 0)
		return 0;

	CRect rect;
	DrawTextW(CUIComDC(m_hFont), m_strText.c_str(), iPos, rect, DT_NOPREFIX | DT_SINGLELINE | DT_CALCRECT);
	return rect.Width();
}

void CUIEdit::CalcSelRect(int iStart, int iEnd, LPRECT lpRect) const
{
	if (iStart == iEnd)
	{
		*lpRect = CRect();
		return;
	}

	CRect rect(CalcPosLeft(iStart), 0, CalcPosLeft(iEnd), m_nHeight);
	rect.NormalizeRect();
	rect.OffsetRect(m_rect.left - m_nLeft, (m_rect.top + m_rect.bottom - m_nHeight) / 2);
	*lpRect = rect;
}

void CUIEdit::InvalidateSel(int iStart, int iEnd)
{
	CRect rect;
	CalcSelRect(iStart, iEnd, rect);
	InvalidateRect(rect);
}

void CUIEdit::OnKeyDown(UINT nChar)
{
	switch (nChar)
	{
	case VK_TAB:
		GetRootView()->NextTabsEdit();
		break;

	case VK_LEFT:
		OnKeyMove(-1);
		break;

	case VK_RIGHT:
		OnKeyMove(1);
		break;

	case VK_HOME:
		OnKeyMove(-2);
		break;

	case VK_END:
		OnKeyMove(2);
		break;

	case VK_BACK:
		if (m_iStart != m_iEnd)
		{
			DelStartEnd(m_iStart, m_iEnd);
		}
		else if (m_iEnd)
		{
			DelStartEnd(m_iEnd - 1, m_iEnd);
		}
		break;

	case VK_DELETE:
		if (m_iStart != m_iEnd)
		{
			DelStartEnd(m_iStart, m_iEnd);
		}
		else if (m_iEnd != m_strText.size())
		{
			DelStartEnd(m_iEnd, m_iEnd + 1);
		}
		break;

	case VK_RETURN:
		if (m_fnOnReturn)
			m_fnOnReturn();
		break;

	default:
		if (GetKeyState(VK_CONTROL) < 0 && GetKeyState(VK_MENU) >= 0)
		{
			switch (nChar)
			{
			case 'Z':
				OnUndo();
				break;

			case 'X':
				OnCut();
				break;

			case 'C':
				OnCopy();
				break;

			case 'V':
				OnPaste();
				break;

			case 'A':
				OnSelAll();
				break;
			}
		}
	}
}

void CUIEdit::OnKeyMove(int nMove)
{
	int nSize = m_strText.size();
	if (nSize == 0)
		return;

	int iEnd = m_iEnd;

	if (nMove < 0 && m_iEnd)
	{
		if (nMove == -1)
			iEnd--;
		else
			iEnd = 0;
	}

	if (nMove > 0 && m_iEnd != nSize)
	{
		if (nMove == 1)
			iEnd++;
		else
			iEnd = nSize;
	}

	if (GetKeyState(VK_SHIFT) < 0)
	{
		InvalidateSel(iEnd, m_iEnd);
		m_iEnd = iEnd;
	}
	else
	{
		InvalidateSel(m_iStart, m_iEnd);
		m_iStart = m_iEnd = iEnd;
	}

	MySetCaretPos();
}

void CUIEdit::DelStartEnd(int iStart, int iEnd, bool bChanged)
{
	if (iStart == iEnd || m_bReadonly)
		return;

	if (iStart > iEnd)
		std::swap(iStart, iEnd);

	wstring strDiff = m_strText.substr(iStart, iEnd - iStart);
	if (m_undo.m_nPos == iStart && m_undo.m_nLen == 0 && iEnd <= (int)m_strText.size())
	{
		m_undo.m_strDiff.append(strDiff);
	}
	else if (m_undo.m_nPos == iEnd)
	{
		m_undo.SetPosLen(iStart);
		m_undo.m_strDiff.insert(0, strDiff);
	}
	else
	{
		m_undo.SetPosLen(iStart);
		m_undo.m_strDiff = strDiff;
	}

	InvalidateSel(iStart, -1);
	m_strText.erase(iStart, iEnd - iStart);
	m_iStart = m_iEnd = iStart;

	if (!bChanged)
		return;

	OnChanged();
	MySetCaretPos();
}

void CUIEdit::OnKeyChar(UINT nChar)
{
	if (nChar < ' ')
		return;

#ifndef _UNICODE
	if (nChar > 127)
	{
		int nTemp = (nChar << 8) | (nChar >> 8);
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)&nTemp, 2, (LPWSTR)&nChar, 1);
	}
#endif

	Insert((LPWSTR)&nChar);
}

void CUIEdit::OnUndo()
{
	if (m_undo.m_nLen == 0 && m_undo.m_strDiff.empty() || m_bReadonly)
		return;

	InvalidateSel(m_iStart, m_iEnd);
	m_iStart = m_undo.m_nPos;
	wstring strDiff = m_strText.substr(m_iStart, m_undo.m_nLen);

	InvalidateSel(m_iStart, -1);
	m_strText.erase(m_iStart, m_undo.m_nLen);
	m_strText.insert(m_iStart, m_undo.m_strDiff);
	InvalidateSel(m_iStart, -1);

	m_iEnd = m_iStart + (m_undo.m_nLen = m_undo.m_strDiff.size());
	m_undo.m_strDiff = strDiff;

	OnChanged();
	MySetCaretPos();
}

void CUIEdit::OnCut()
{
	if (m_iStart == m_iEnd || m_bPassword || m_bReadonly)
		return;

	OnCopy();
	DelStartEnd(m_iStart, m_iEnd);
}

void CUIEdit::OnCopy()
{
	if (m_iStart == m_iEnd || m_bPassword)
		return;

	if (!OpenClipboard(NULL))
		return;

	int iStart = min(m_iStart, m_iEnd);
	int iEnd   = m_iStart ^ m_iEnd ^ iStart;
	int nSize  = iEnd - iStart;

	HGLOBAL	hGlobal = GlobalAlloc(GMEM_MOVEABLE, (nSize + 1) * 2);
	wcsncpy_s((LPWSTR)GlobalLock(hGlobal), nSize + 1, m_strText.c_str() + iStart, nSize);
	GlobalUnlock(hGlobal);

	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hGlobal);
	CloseClipboard();
}

void CUIEdit::OnPaste()
{
	if (!OpenClipboard(NULL))
		return;

	if (HANDLE hGlobal = GetClipboardData(CF_UNICODETEXT))
	{
		LPWSTR lpSrc = (LPWSTR)GlobalLock(hGlobal);
		LPWSTR lpDst = (LPWSTR)alloca((wcslen(lpSrc) + 1) * 2), lp = lpDst;

		while (int ch = *lpSrc++)
		{
			if (ch >= ' ' && (!m_bPassword || ch <= 127))
				*lp++ = ch;
		}

		*lp = 0;
		GlobalUnlock(hGlobal);
		CloseClipboard();
		Insert(lpDst);
	}
	else
		CloseClipboard();
}

void CUIEdit::OnSelAll()
{
	if (m_iStart > m_iEnd)
		std::swap(m_iStart, m_iEnd);

	InvalidateSel(0, m_iStart);
	InvalidateSel(m_iEnd, -1);

	m_iStart = 0;
	m_iEnd   = m_strText.size();
	MySetCaretPos();
}

void CUIEdit::Insert(LPWSTR lpText)
{
	if (m_bReadonly)
		return;

	if (m_fnFilter && lpText[0])
	{
		m_fnFilter(lpText);

		if (lpText[0] == 0)
			return;
	}

	if (m_iStart != m_iEnd)
		DelStartEnd(m_iStart, m_iEnd, false);

	if (m_undo.m_nPos + m_undo.m_nLen != m_iEnd)
	{
		m_undo.SetPosLen(m_iEnd);
		m_undo.m_strDiff.clear();
	}

	CheckMaxText(m_strText.c_str(), lpText);

	m_strText.insert(m_iEnd, lpText);
	InvalidateSel(m_iEnd, -1);
	m_iStart = m_iEnd += wcslen(lpText);
	m_undo.m_nLen = m_iEnd - m_undo.m_nPos;

	OnChanged();
	MySetCaretPos();
}

void CUIEdit::MyCreateCaret()
{
	HWND hWnd = GetRootView()->GetHwnd();
	HIMC hImc = ImmGetContext(hWnd);

	if (hImc)
	{
		LOGFONTW lf;
		GetObjectW(m_hFont, sizeof(lf), &lf);
		ImmSetCompositionFontW(hImc, &lf);
		ImmReleaseContext(hWnd, hImc);
	}

	m_ptCaret = CPoint(MAXINT16, MAXINT16);
	MySetCaretPos();
}

void CUIEdit::MyDestroyCaret()
{
	if (m_bCaret)
		OnUITimer();

	m_uiTimer.Kill();
}

void CUIEdit::CalcCaretPos(LPPOINT lpPoint)
{
	CPoint point(m_rect.left + CalcPosLeft(m_iEnd) - m_nLeft, (m_rect.top + m_rect.bottom - m_nHeight) / 2);

	if (point.x < m_rect.left)
	{
		// 光标超出左边界，整体右移
		while (point.x < m_rect.left)
		{
			int nSize = m_rect.Width() / 4;
			point.x += nSize;
			m_nLeft -= nSize;
		}

		InvalidateRect(NULL);
	}
	else if (point.x >= m_rect.right)
	{
		// 光标超出右边界，整体左移
		while (point.x >= m_rect.right)
		{
			int nSize = m_rect.Width() / 4;
			point.x -= nSize;
			m_nLeft += nSize;
		}

		InvalidateRect(NULL);
	}

	if (m_nLeft > 0)
	{
		// 移除文字右空白
		int nMore = m_rect.Width() + m_nLeft - CalcPosLeft(-1) - 1;
		if (nMore > 0)
		{
			point.x += nMore;
			m_nLeft -= nMore;
			InvalidateRect(NULL);
		}
	}

	if (m_nLeft < 0)
	{
		// 移除文字左空白
		point.x += m_nLeft;
		m_nLeft = 0;
		InvalidateRect(NULL);
	}

	*lpPoint = point;
}

void CUIEdit::MySetCaretPos()
{
	if (m_rect.IsRectEmpty())
		return;

	CPoint point;
	CalcCaretPos(&point);

	if (m_ptCaret == point || !m_bFocus)
		return;

	if (m_bCaret)
		OnUITimer();

	m_uiTimer.Set(GetCaretBlinkTime());
	m_ptCaret = point;
	OnUITimer();

	HWND hWnd = GetRootView()->GetHwnd();
	HIMC hImc = ImmGetContext(hWnd);

	if (hImc)
	{
		COMPOSITIONFORM compForm = { CFS_POINT, point };
		ImmSetCompositionWindow(hImc, &compForm);
		ImmReleaseContext(hWnd, hImc);
	}
}

void CUIEdit::OnUITimer()
{
	m_bCaret = !m_bCaret;
	InvalidateRect(CRect(m_ptCaret, CSize(1, m_nHeight)));
}

void CUIEdit::CheckMaxText(LPCWSTR lpExist, LPWSTR lpText) const
{
	int nMaxLen = (WORD)m_nMaxLen;
	if (nMaxLen == 0)
		return;

	// 字符数
	if (m_nMaxLen >> 16 == 0)
	{
		int nSize = nMaxLen - wcslen(lpExist);
		if (nSize < (int)wcslen(lpText))
			lpText[max(nSize, 0)] = 0;
		return;
	}

	// 字节数
	while (WORD ch = *lpExist++)
	{
		if (ch > 127)
			nMaxLen -= 2;
		else
			nMaxLen--;
	}

	while (WORD ch = *lpText)
	{
		if (ch > 127)
			nMaxLen -= 2;
		else
			nMaxLen--;

		if (nMaxLen < 0)
			*lpText = 0;
		else
			lpText++;
	}
}

void CUIEdit::OnLoaded(const IUILoadAttrs &attrs)
{
	__super::OnLoaded(attrs);

	LPCWSTR lpStr;
	if (lpStr = attrs.GetStr(L"font"))
		SetFont(attrs.GetFont(lpStr));

	if (lpStr = attrs.GetStr(L"color"))
	{
		COLORREF color = 0;
		ATLVERIFY(IsStrColor(lpStr, &color));
		SetTextColor(color);
	}

	int nValue;
	if (attrs.GetInt(L"maxBytes", &nValue))
		SetMaxBytes(nValue);
	else if (attrs.GetInt(L"maxChars", &nValue))
		SetMaxChars(nValue);

	if (attrs.GetInt(L"password", &nValue))
		SetPassword(nValue != 0);

	if (attrs.GetInt(L"readonly", &nValue))
		SetReadonly(nValue != 0);

	if (lpStr = attrs.GetStr(L"text"))
		SetText(lpStr);
}
