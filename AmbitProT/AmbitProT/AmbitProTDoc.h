// AmbitProTDoc.h : interface of the CAmbitProTDoc class
//


#pragma once


class CAmbitProTDoc : public CDocument
{
protected: // create from serialization only
	CAmbitProTDoc();
	DECLARE_DYNCREATE(CAmbitProTDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CAmbitProTDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


