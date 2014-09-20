#include "StdAfx.h"
#include "MySplitterWnd.h"

CMySplitterWnd::CMySplitterWnd(void)
{
}

CMySplitterWnd::~CMySplitterWnd(void)
{
}
BEGIN_MESSAGE_MAP(CMySplitterWnd, CSplitterWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CMySplitterWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	return ;

	//CSplitterWnd::OnMouseMove(nFlags, point);
}

void CMySplitterWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	return ;

	//CSplitterWnd::OnLButtonDown(nFlags, point);
}
