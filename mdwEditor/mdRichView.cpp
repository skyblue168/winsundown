// mdRichView.cpp : implementation file
//

#include "stdafx.h"
#include "mdwEditor.h"
#include "mdRichView.h"


// CmdRichView

IMPLEMENT_DYNCREATE(CmdRichView, CRichEditView)

CmdRichView::CmdRichView()
{

}

CmdRichView::~CmdRichView()
{
}

BEGIN_MESSAGE_MAP(CmdRichView, CRichEditView)
END_MESSAGE_MAP()


// CmdRichView diagnostics

#ifdef _DEBUG
void CmdRichView::AssertValid() const
{
	CRichEditView::AssertValid();
}

#ifndef _WIN32_WCE
void CmdRichView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}
#endif
#endif //_DEBUG


// CmdRichView message handlers
