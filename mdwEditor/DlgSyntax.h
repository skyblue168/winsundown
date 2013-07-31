#pragma once


// CDlgSyntax dialog

class CDlgSyntax : public CDialog
{
	DECLARE_DYNAMIC(CDlgSyntax)

public:
	CDlgSyntax(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSyntax();

// Dialog Data
	enum { IDD = IDD_DLGHELP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
//	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	TCHAR m_Hotkey;
	virtual BOOL OnInitDialog();
//	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
//	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
