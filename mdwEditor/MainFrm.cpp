
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "mdwEditor.h"

#include "mdwEditorDoc.h"
#include "mdcodeView.h"
#include "mdRichView.h"
#include "mdwEditorView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
//	ON_WM_KEYUP()
//ON_COMMAND(ID_EDIT_NEXTWIN, &CMainFrame::OnEditNextwin)
//ON_WM_KEYDOWN()
ON_WM_SIZE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bFirstOnSize = TRUE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	//ShowWindow(SW_SHOWMAXIMIZED);

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	//return m_wndSplitter.Create(this,
	//	2, 2,               // TODO: adjust the number of rows, columns
	//	CSize(10, 10),      // TODO: adjust the minimum pane size
	//	pContext, WS_CHILD | WS_VISIBLE | SPLS_DYNAMIC_SPLIT);

	CRect rt;
	this->GetClientRect(rt);

	BOOL rc = m_wndSplitter.CreateStatic(this, 1, 2);

	m_wndSplitter.CreateView(0,0, RUNTIME_CLASS(CmdcodeView), CSize(rt.Width()/2, 0), pContext);
	m_wndSplitter.CreateView(0,1, RUNTIME_CLASS(CmdwEditorView), CSize(0, 0), pContext);

	SetActiveView((CView*)m_wndSplitter.GetPane(0,0));

	return rc;
}


void CMainFrame::changeActiveView()
{
	int cxCur, cxMin;
	CView *cvAct, *cvNext;

	cvAct = this->GetActiveView();

	m_wndSplitter.GetColumnInfo(1, cxCur, cxMin);
	m_wndSplitter.SetColumnInfo(0, cxCur, cxMin);
	m_wndSplitter.RecalcLayout();

	if((CView*)m_wndSplitter.GetPane(0,0) == cvAct) {
		cvNext = (CView*)m_wndSplitter.GetPane(0,1);
	}
	else {
		cvNext = (CView*)m_wndSplitter.GetPane(0,0);
	}

	SetActiveView(cvNext);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{

	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	//cs.style |= WS_VISIBLE|WS_MAXIMIZE;

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	if(m_bFirstOnSize) {
		m_bFirstOnSize = FALSE;
	}
	else {
		int cxCur, cxMin;
		m_wndSplitter.GetColumnInfo(1, cxCur, cxMin);
		m_wndSplitter.SetColumnInfo(0, cx/2, cxMin);
		m_wndSplitter.RecalcLayout();
	}

}
