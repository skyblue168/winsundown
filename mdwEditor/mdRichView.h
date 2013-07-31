#pragma once


// CmdRichView view

class CmdRichView : public CRichEditView
{
	DECLARE_DYNCREATE(CmdRichView)

protected:
	CmdRichView();           // protected constructor used by dynamic creation
	virtual ~CmdRichView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


