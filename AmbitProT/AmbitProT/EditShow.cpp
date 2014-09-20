// EditShow.cpp : implementation file
//

#include "stdafx.h"
#include "AmbitProT.h"
#include "EditShow.h"



#define CONSOLE_BUF_SIZE 200000
// CEditShow

IMPLEMENT_DYNAMIC(CEditShow, CEdit)

CEditShow::CEditShow()
{
	

}

CEditShow::~CEditShow()
{
}


BEGIN_MESSAGE_MAP(CEditShow, CEdit)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CEditShow message handlers


///////////display the data ////////////
bool CEditShow::ShowInfo(CString str)
{
	int Len=0;

	Len=str.GetLength();
	if (Len<=CONSOLE_BUF_SIZE)
	{
		int nLength=GetWindowTextLength();	
		if (nLength>=CONSOLE_BUF_SIZE)//if the data size more than control buf size, clear the control buffer.
		{
			CString strTemp;
			strTemp.Format(_T("Data size > %d, reset buffer"), CONSOLE_BUF_SIZE);
			SetWindowText(strTemp);
		}
		else
		{
			SetSel(nLength,nLength);
			ReplaceSel((LPCTSTR)str);
		}	
	}
	else
	{
		CString strTemp;
		strTemp.Format(_T("Data size > %d, reset buffer"), CONSOLE_BUF_SIZE);
		SetWindowText(strTemp);
	}
	return true;

}
///////////////..................////////////

///////clear the view........//////////
bool CEditShow::Clear(void)
{
	SetWindowText(_T(""));
	return true;
}
////////////////............./////////

int CEditShow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;	
	m_Font.CreateFont(
		14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial"));                 
	SetFont(&m_Font); 
	SetLimitText(CONSOLE_BUF_SIZE);//Set edit text buf


	return 0;
}

int CEditShow::GetRow(char* pstrinfor)
{
	//return int(size.cx/(rec.Width()+18)+1);
	return 1;
}


void CEditShow::OnSize(UINT nType, int cx, int cy)
{
	CEdit::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}


void CEditShow::ClearSHowBuf(void)
{
	//memset(showbuf,0,sizeof(showbuf));
}

void CEditShow::TurntoBottom(void)
{
	int i=GetLineCount();   
	LineScroll(i,0);  
}
