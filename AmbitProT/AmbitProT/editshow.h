#pragma once


// CEditShow

class CEditShow : public CEdit
{
	DECLARE_DYNAMIC(CEditShow)

public:
	CEditShow();
	virtual ~CEditShow();

	
	CFont m_Font;

protected:
	DECLARE_MESSAGE_MAP()
public:
	bool ShowInfo(CString str);
public:
	bool Clear(void);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	int GetRow(char* pstrinfor);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	void ClearSHowBuf(void);
public:
	void TurntoBottom(void);
};


