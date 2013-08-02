// mdcodeView.cpp : implementation file
//

#include "stdafx.h"
#include "mdwEditor.h"
#include "mdcodeView.h"
#include "MainFrm.h"
#include "DlgSyntax.h"

// CmdcodeView

IMPLEMENT_DYNCREATE(CmdcodeView, CEditView)

CmdcodeView::CmdcodeView()
{
	m_Font.CreateFont( 
		22, 0, 0, 0, 
		FW_NORMAL,
   		FALSE, FALSE, FALSE, 0, 
		OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS,   
		DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_ROMAN, 
		_T("Verdana")
	);

	m_ctrlQ = FALSE;
}

CmdcodeView::~CmdcodeView()
{
}

BEGIN_MESSAGE_MAP(CmdcodeView, CEditView)
	ON_WM_CREATE()
//	ON_WM_ACTIVATE()
ON_WM_KEYUP()
//ON_WM_CHAR()
ON_WM_KEYDOWN()
//ON_COMMAND(ID_EDIT_HELP, &CmdcodeView::OnEditHelp)
END_MESSAGE_MAP()


// CmdcodeView diagnostics

#ifdef _DEBUG
void CmdcodeView::AssertValid() const
{
	CEditView::AssertValid();
}

#ifndef _WIN32_WCE
void CmdcodeView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif
#endif //_DEBUG


// CmdcodeView message handlers

int CmdcodeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEditView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	this->SetFont(&m_Font);
	this->GetEditCtrl().SetLimitText(1024*1024*4);
	return 0;
}

void CmdcodeView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: Add your specialized code here and/or call the base class

	CEditView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	TRACE("CodeView activate = %d\n", bActivate);
}

void CmdcodeView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if(m_ctrlQ) {
		m_ctrlQ = FALSE;
		switch(nChar) {
			case 'Q':
				openHelpDialog();
				break;
			case 'C':
			case 'G':
			case 'L':
			case 'R':
			case 'T':
				insertMarkdownCode(nChar);
				break;
		}
	}

	CEditView::OnKeyUp(nChar, nRepCnt, nFlags);

}

void CmdcodeView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if(nChar == VK_TAB) {
		SHORT rc = GetKeyState(VK_CONTROL);
		if( rc & 0x80 ) {
			TRACE("ctrl-tab is %d\n", rc );
			CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
			pFrame->changeActiveView();
			return;
		}
	}
	else if(nChar == 'Q') {
		if( 0x80 & GetKeyState(VK_CONTROL) ) {
			m_ctrlQ = TRUE;
		}
	}

	CEditView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CmdcodeView::insertMarkdownCode(TCHAR key)
{
	BOOL isOK = TRUE;
	CString sCode;

	switch(key) {
		case _T('c'):
		case _T('C'):
			sCode = _T("```\r\n    \r\n```\r\n");
			break;

		case _T('g'):
		case _T('G'):
			sCode = _T("![ImageText](http://)");
			break;

		case _T('l'):
		case _T('L'):
			sCode = _T("[LinkText](http://)");
			break;

		case _T('r'):
		case _T('R'):
			sCode = _T("----------------------------------------\r\n");
			break;

		case _T('t'):
		case _T('T'):
			sCode = _T("head1|head2|head3\r\n-----|-----|-----\r\n");
			break;

		default:
			isOK = FALSE;
			break;
	}

	if(isOK) {
		int nBeg, nEnd;

		this->GetEditCtrl().GetSel(nBeg, nEnd);
		this->GetEditCtrl().SetSel(nBeg, nEnd);
		this->GetEditCtrl().ReplaceSel(sCode);
	}
}

void CmdcodeView::openHelpDialog()
{
	// TODO: Add your command handler code here
	CDlgSyntax dlg;

	if(dlg.DoModal() == IDOK) {
		if( dlg.m_Hotkey != 0) 
			insertMarkdownCode(dlg.m_Hotkey);
	}
}
