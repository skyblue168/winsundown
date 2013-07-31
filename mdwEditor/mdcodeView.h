#pragma once


// CmdcodeView view

class CmdcodeView : public CEditView
{
	DECLARE_DYNCREATE(CmdcodeView)

	CFont m_Font;
	BOOL  m_ctrlQ;

	void insertMarkdownCode(TCHAR key);
	void openHelpDialog();

protected:
	CmdcodeView();           // protected constructor used by dynamic creation
	virtual ~CmdcodeView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
public:
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//	afx_msg void OnEditHelp();
};


