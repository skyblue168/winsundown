// DlgSyntax.cpp : implementation file
//

#include "stdafx.h"
#include "mdwEditor.h"
#include "DlgSyntax.h"


// CDlgSyntax dialog

IMPLEMENT_DYNAMIC(CDlgSyntax, CDialog)

CDlgSyntax::CDlgSyntax(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSyntax::IDD, pParent)
	, m_Hotkey(0)
{

}

CDlgSyntax::~CDlgSyntax()
{
}

void CDlgSyntax::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSyntax, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSyntax::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgSyntax::OnBnClickedCancel)
//	ON_WM_KEYUP()
//	ON_WM_CHAR()
END_MESSAGE_MAP()


// CDlgSyntax message handlers

void CDlgSyntax::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CDlgSyntax::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

BOOL CDlgSyntax::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_Hotkey = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CDlgSyntax::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYUP) {
		BOOL isOK = TRUE;

		switch(pMsg->wParam) {
			case _T('c'):
			case _T('C'):
				m_Hotkey = _T('C');
				break;

			case _T('g'):
			case _T('G'):
				m_Hotkey = _T('G');
				break;

			case _T('l'):
			case _T('L'):
				m_Hotkey = _T('L');
				break;

			case _T('r'):
			case _T('R'):
				m_Hotkey = _T('R');
				break;

			case _T('t'):
			case _T('T'):
				m_Hotkey = _T('T');
				break;

			default:
				isOK = FALSE;
				break;
		}
		if(isOK)
			OnOK();
	}

	return CDialog::PreTranslateMessage(pMsg);
}
