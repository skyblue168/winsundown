
// mdwEditorView.cpp : implementation of the CmdwEditorView class
//

#include "stdafx.h"
#include "mdwEditor.h"

#include "mdwEditorDoc.h"
#include "mdwEditorView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CmdwEditorView

IMPLEMENT_DYNCREATE(CmdwEditorView, CHtmlView)

BEGIN_MESSAGE_MAP(CmdwEditorView, CHtmlView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CHtmlView::OnFilePrint)
//	ON_WM_ACTIVATE()
ON_WM_KEYDOWN()
//ON_WM_CHAR()
END_MESSAGE_MAP()

// CmdwEditorView construction/destruction

CmdwEditorView::CmdwEditorView()
{
	// TODO: add construction code here

}

CmdwEditorView::~CmdwEditorView()
{
}

BOOL CmdwEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CHtmlView::PreCreateWindow(cs);
}

void CmdwEditorView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

	//Navigate2(_T("http://www.msdn.microsoft.com/visualc/"),NULL,NULL);
}


// CmdwEditorView printing



// CmdwEditorView diagnostics

#ifdef _DEBUG
void CmdwEditorView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CmdwEditorView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

CmdwEditorDoc* CmdwEditorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CmdwEditorDoc)));
	return (CmdwEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CmdwEditorView message handlers

void CmdwEditorView::OnUpdate(CView* pSender, LPARAM lHint, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class
	if(lHint != 0)
		Navigate2((LPCTSTR)lHint, NULL, NULL);
}

void CmdwEditorView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: Add your specialized code here and/or call the base class

	CHtmlView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CmdwEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	//if(nChar == VK_TAB) {
	//	SHORT rc = GetKeyState(VK_CONTROL);
	//	if( rc & 0x80 ) {
	//		TRACE("ctrl-tab is %d\n", rc );
	//		CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	//		pFrame->changeActiveView();
	//		return;
	//	}
	//}

	CHtmlView::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CmdwEditorView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message==WM_KEYDOWN ) { 
		if( pMsg->wParam == VK_TAB) {
			SHORT rc = GetKeyState(VK_CONTROL);
			if( rc & 0x80 ) {
				TRACE("PreTrans ctrl-tab is %d\n", rc );
				CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
				pFrame->changeActiveView();
				return TRUE;
			}
		}
	} 

	return CHtmlView::PreTranslateMessage(pMsg);
}
