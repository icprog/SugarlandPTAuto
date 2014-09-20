#pragma once
#include "afxext.h"

class CMySplitterWnd :
	public CSplitterWnd
{
public:
	CMySplitterWnd(void);
public:
	~CMySplitterWnd(void);
public:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
