// AmbitProTDoc.cpp : implementation of the CAmbitProTDoc class
//

#include "stdafx.h"
#include "AmbitProT.h"

#include "AmbitProTDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAmbitProTDoc

IMPLEMENT_DYNCREATE(CAmbitProTDoc, CDocument)

BEGIN_MESSAGE_MAP(CAmbitProTDoc, CDocument)
END_MESSAGE_MAP()


// CAmbitProTDoc construction/destruction

CAmbitProTDoc::CAmbitProTDoc()
{
	// TODO: add one-time construction code here

}

CAmbitProTDoc::~CAmbitProTDoc()
{
}

BOOL CAmbitProTDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CAmbitProTDoc serialization

void CAmbitProTDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CAmbitProTDoc diagnostics

#ifdef _DEBUG
void CAmbitProTDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAmbitProTDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CAmbitProTDoc commands
