#include "stdafx.h"
#include "StaticEx.h"


CStaticEx::CStaticEx()
{
}

CStaticEx::CStaticEx(bool is_hyperlink)
{
	m_isHyperLink = is_hyperlink;
}


CStaticEx::~CStaticEx()
{
}

void CStaticEx::SetWindowTextEx(LPCTSTR lpszString)
{
	CDC* pDC = GetDC();
	pDC->SetTextColor(m_TextColor);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SelectObject(this->GetFont());
	//int nTextLeft = 0, nTextTop = 0; //文字输出的位置
	CRect rect;
	this->GetClientRect(&rect);
	DrawThemeParentBackground(m_hWnd, pDC->GetSafeHdc(), &rect);	//重绘控件区域以解决文字重叠的问题
	pDC->DrawText(lpszString, rect, DT_VCENTER | DT_SINGLELINE);
	ReleaseDC(pDC);
	m_text = lpszString;
}

void CStaticEx::SetTextColor(COLORREF textColor)
{
	m_TextColor = textColor;
	SetWindowTextEx(m_text);
}

CString CStaticEx::GetString() const
{
	return m_text;
}

void CStaticEx::SetURL(CString strURL)
{
	m_strURL = strURL;
}

CString CStaticEx::GetURL() const
{
	return m_strURL;
}


LRESULT CStaticEx::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (message == WM_SETTEXT)
	{
		CRect rect;
		CDC* pDC = GetDC();
		GetClientRect(rect);
		DrawThemeParentBackground(m_hWnd, pDC->GetSafeHdc(), &rect);
		ReleaseDC(pDC);
	}
	return CStatic::DefWindowProc(message, wParam, lParam);
}
BEGIN_MESSAGE_MAP(CStaticEx, CStatic)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


void CStaticEx::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_isHyperLink)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		_TrackMouseEvent(&tme);
	}
	else
	{
		CStatic::OnMouseMove(nFlags, point);
	}
}


void CStaticEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_isHyperLink)
	{
		if (m_linkEnable)
		{
			if (m_strURL.IsEmpty())
				m_strURL = m_text;
			ShellExecute(NULL, _T("open"), m_strURL, NULL, NULL, SW_SHOW);	//打开超链接
		}
		else
		{
			CWnd* pParent{ GetParent() };
			if(pParent!=nullptr)
				pParent->SendMessage(WM_LINK_CLICKED, (WPARAM)this);
		}
	}
	else
	{
		CStatic::OnLButtonUp(nFlags, point);
	}
}


void CStaticEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CStatic::OnPaint()
	if (m_isHyperLink)
	{
		CFont* pFont = GetFont();
		CFont m_Font;
		if (pFont != NULL)
		{
			LOGFONT lf;
			pFont->GetLogFont(&lf);
			lf.lfUnderline = m_bHot;
			if (m_Font.CreateFontIndirect(&lf))
				dc.SelectObject(m_Font);
		}
		dc.SetTextColor(RGB(0, 0, 255));
		dc.SetBkMode(TRANSPARENT);
		///准备工作
		CRect rect;
		CBrush BGBrush, *pOldBrush;
		this->GetClientRect(&rect);
		//画背景
		COLORREF backColor = GetSysColor(COLOR_BTNFACE);
		BGBrush.CreateSolidBrush(backColor);
		pOldBrush = dc.SelectObject(&BGBrush);
		dc.FillRect(&rect, &BGBrush);
		dc.SelectObject(pOldBrush);
		BGBrush.DeleteObject();
		///输出文字
		if (m_text.GetLength()>0)
		{
			dc.DrawText(m_text, rect, DT_VCENTER | DT_SINGLELINE);
		}
		m_Font.DeleteObject();
	}
}


void CStaticEx::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_isHyperLink && !m_bHot)
	{
		m_bHot = true;
		Invalidate();
	}
	else
	{
		CStatic::OnMouseHover(nFlags, point);
	}
}


void CStaticEx::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_isHyperLink)
	{
		m_bHot = false;
		Invalidate();
	}
	else
	{
		CStatic::OnMouseLeave();
	}
}


BOOL CStaticEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_isHyperLink && m_bHot)
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649)));
		return TRUE;
	}

	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}


void CStaticEx::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	if (m_isHyperLink)
	{
		DWORD dwStyle = GetStyle();
		::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
	}
	GetWindowText(m_text);

	CStatic::PreSubclassWindow();
}
