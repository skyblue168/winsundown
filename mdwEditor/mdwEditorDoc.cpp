
// mdwEditorDoc.cpp : implementation of the CmdwEditorDoc class
//

#include "stdafx.h"
#include "mdwEditor.h"

#include "mdwEditorDoc.h"

#include "markdown.h"
#include "html.h"
#include "buffer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define READ_UNIT 1024
#define OUTPUT_UNIT 64

static const char UTF8_BOM[] = {0xEF, 0xBB, 0xBF};

struct buf * mib_utf8 = NULL;

// CmdwEditorDoc

IMPLEMENT_DYNCREATE(CmdwEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CmdwEditorDoc, CDocument)
	ON_COMMAND(ID_FILE_EXPORT, &CmdwEditorDoc::OnFileExport)
END_MESSAGE_MAP()


char* unicode2Utf8(LPCTSTR unicode, UINT nLen)
{
	char* outb = NULL;
	int len = WideCharToMultiByte(CP_UTF8, 0, unicode, nLen, NULL, 0, NULL, NULL);

	outb = (char*)malloc(len+1);
	memset(outb, 0, sizeof(outb));
	int ret = WideCharToMultiByte(CP_UTF8, 0, unicode, nLen, (LPSTR)outb, len, NULL, NULL);
	outb[ret] = 0;
	return outb;
}

void getEditText(LPCTSTR unicode, UINT nLen)
{
	int idx = 3;
    int len = WideCharToMultiByte(CP_UTF8, 0, unicode, nLen, NULL, 0, NULL, NULL);  

	mib_utf8 = bufnew(READ_UNIT);
	bufgrow(mib_utf8, len+idx+1);
	memset(mib_utf8->data, 0, mib_utf8->asize);
	memcpy(mib_utf8->data, UTF8_BOM, idx);
	mib_utf8->size += idx;

	mib_utf8->size += WideCharToMultiByte(CP_UTF8, 0, unicode, nLen, (LPSTR)&mib_utf8->data[idx], len, NULL, NULL);
}

void setEditText(const char* src, int dlen, CEditView* pv)
//LPWSTR utf8ToUnicode(const char* src, int dlen)  
{  
    int nLen = MultiByteToWideChar(CP_UTF8, 0, src, dlen, NULL, 0 );  

	LPVOID hText = LocalAlloc(LMEM_MOVEABLE, static_cast<UINT>(::ATL::AtlMultiplyThrow(static_cast<UINT>(nLen+1),static_cast<UINT>(sizeof(TCHAR)))));
	if (hText == NULL)
		AfxThrowMemoryException();

	LPTSTR lpszText = (LPTSTR)LocalLock(hText);
	ASSERT(lpszText != NULL);

	MultiByteToWideChar(CP_UTF8, 0, src, dlen, lpszText, nLen);  
	// Replace the editing edit buffer with the newly loaded data
	lpszText[nLen] = '\0';
    
	LocalUnlock(hText);
	HLOCAL hOldText = pv->GetEditCtrl().GetHandle();
	ASSERT(hOldText != NULL);
	LocalFree(hOldText);
	pv->GetEditCtrl().SetHandle((HLOCAL)hText);

}

BOOL readUtf8Data(CFile* pCF)
{
	int ret;

	if(pCF == NULL)	return FALSE;

	if(mib_utf8 != NULL) {
		bufrelease(mib_utf8);
		mib_utf8 = NULL;
	}

	/* reading everything */
	mib_utf8 = bufnew(READ_UNIT);
	bufgrow(mib_utf8, READ_UNIT);

	while ((ret = pCF->Read(mib_utf8->data + mib_utf8->size, mib_utf8->asize - mib_utf8->size)) > 0) {
		mib_utf8->size += ret;
		bufgrow(mib_utf8, mib_utf8->size + READ_UNIT);
	}

	return TRUE;
}

//void my_doc_header(struct buf *ob, void *opaque)
//{
//	char hHead[] = "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=""utf-8"">\n";
//	char hCSS[] = "<link href=""http://github.com/yrgoldteeth/darkdowncss/raw/master/darkdown.css"" rel=""stylesheet""></link>\n</head>\n<body>\n";
//}
//
//void my_doc_footer)(struct buf *ob, void *opaque)
//{
//	char hTail[] = "</body>\n</html>";
//}

BOOL convMd2Html(LPCTSTR outpath, const CString& csspath)
{
	struct buf *ob;
	int ret;
	FILE *outf = NULL;
	unsigned int extensions = 0xff;
	char hHead[] = "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=""utf-8"">\n";
	char hSym[] = "<link href=""%s"" rel=""stylesheet""></link>\n</head>\n<body>\n";
	char hTail[] = "</body>\n</html>";
	char *hCSS = NULL;

	struct sd_callbacks callbacks;
	struct html_renderopt options;
	struct sd_markdown *markdown;

	char *cssbuf = unicode2Utf8((LPCTSTR)csspath, csspath.GetLength());
	int aclen = strlen(cssbuf) + strlen(hSym);

	hCSS = (char*)malloc(aclen+1);
	memset(hCSS, 0, aclen);
	sprintf(hCSS, "<link href=""%s"" rel=""stylesheet""></link>\n</head>\n<body>\n", cssbuf);
	free(cssbuf);


	/* performing markdown parsing */
	ob = bufnew(OUTPUT_UNIT);

	//extensions = MKDEXT_TABLES|MKDEXT_FENCED_CODE|MKDEXT_AUTOLINK|MKDEXT_STRIKETHROUGH|MKDEXT_SUPERSCRIPT|MKDEXT_LAX_SPACING;

	sdhtml_renderer(&callbacks, &options, 0);
	markdown = sd_markdown_new(extensions, 16, &callbacks, &options);

	sd_markdown_render(ob, mib_utf8->data, mib_utf8->size, markdown);
	sd_markdown_free(markdown);

	outf = _tfopen(outpath, _T("w"));
	if(outf != NULL) {
		fwrite(hHead, 1, strlen(hHead), outf);
		fwrite(hCSS, 1, strlen(hCSS), outf);
		/* writing the result to stdout */
		ret = fwrite(ob->data, 1, ob->size, outf);
		fwrite(hTail, 1, strlen(hTail), outf);
		fclose(outf);
	}

	/* cleanup */
	free(hCSS);
	bufrelease(mib_utf8);
	bufrelease(ob);
	mib_utf8 = NULL;

	return TRUE;
}

void createDefaultCSSFile(const CString& cssPath, const CString& cssData)
{
	char *cssbuf = unicode2Utf8((LPCTSTR)cssData, cssData.GetLength());
	if(cssbuf == NULL)	return;

	FILE *fp = _tfopen(cssPath, _T("w"));
	if(fp != NULL) {
		fwrite(cssbuf, 1, strlen(cssbuf), fp);
		fclose(fp);
	}

	free(cssbuf);
}

// CmdwEditorDoc construction/destruction

CmdwEditorDoc::CmdwEditorDoc()
{
	// TODO: add one-time construction code here
	TCHAR buf[1024];

	::GetModuleFileName(NULL, buf, sizeof(buf));
	TRACE( _T("exe path = %s\n"), buf);

	CString tmp = CString(buf);
	int idx = tmp.ReverseFind('\\');
	if(idx > 0) {
		m_AppPath = tmp.Left(idx+1);
	}
	else {
		m_AppPath = tmp + _T('\\');
		::CreateDirectory(m_AppPath, NULL);
	}

	m_cssPath = m_AppPath + _T("mdwdefault.css");
	m_htmlPath = m_AppPath + _T("preview_001.html");
	m_bHtmlExisted = FALSE;

	checkDefaultCSS();
}

CmdwEditorDoc::~CmdwEditorDoc()
{
}


BOOL CmdwEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

// CmdwEditorDoc serialization

void CmdwEditorDoc::Serialize(CArchive& ar)
{
	POSITION pos = this->GetFirstViewPosition();
	CView* cv = NULL;
	CEditView* pEv = NULL;
	
	do {
		cv = this->GetNextView(pos);
		if( cv != NULL && cv->IsKindOf(RUNTIME_CLASS(CEditView))) {
			pEv = (CEditView*)cv;  
			break;
		}
	} while(cv != NULL);

	if(pEv == NULL) return;

	//CString htmlpath;
	CFile *pCF = ar.GetFile();
	//if(pCF != NULL)
	//	htmlpath = pCF->GetFilePath() + _T(".html");

	if (ar.IsStoring())
	{
		// TODO: add storing code here
		/**
			unicode data 修改後儲存時轉換回 utf-8
			再由sundown 轉換成 html，通知 HtmlView 顯示
		*/
		LPCTSTR lpszText = pEv->LockBuffer();
		ASSERT(lpszText != NULL);
		UINT nLen = pEv->GetBufferLength();
		TRY
		{
			//ar.Write(lpszText, nLen*sizeof(TCHAR));
			getEditText(lpszText, nLen);

			pCF->SeekToBegin();
			pCF->Write(mib_utf8->data, mib_utf8->size);

			m_bHtmlExisted = convMd2Html((LPCTSTR)m_htmlPath, m_cssPath);
			this->UpdateAllViews(NULL, (LPARAM)(LPCTSTR)m_htmlPath, NULL);
		}
		CATCH_ALL(e)
		{
			pEv->UnlockBuffer();
			THROW_LAST();
		}
		END_CATCH_ALL

		pEv->UnlockBuffer();
	}
	else
	{
		// TODO: add loading code here
		/**
			讀入的檔案是utf-8，要轉出一份 unicode data, 設成 CEditView data.
			再由sundown 轉換成 html，通知 HtmlView 顯示
		*/
		
		readUtf8Data( pCF );

		int idx = 0;
		if(mib_utf8->size > 3 && memcmp(mib_utf8->data, UTF8_BOM, 3) == 0)
			idx += 3;

		if(pEv != NULL) {
			setEditText((const char*)&mib_utf8->data[idx], mib_utf8->size-idx, pEv);
		}

		// md conver to html
		m_bHtmlExisted = convMd2Html((LPCTSTR)m_htmlPath, m_cssPath);
		this->UpdateAllViews(NULL, (LPARAM)(LPCTSTR)m_htmlPath, NULL);
	}
}


// CmdwEditorDoc diagnostics

#ifdef _DEBUG
void CmdwEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CmdwEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


void CmdwEditorDoc::checkDefaultCSS()
{
	FILE *fp = NULL;

	fp = _tfopen(m_cssPath, _T("r"));
	if(fp != NULL) {
		fclose(fp);
		return;
	}
	else {

	CString cssData = 
		CString(_T("@charset \"utf-8\";")) +
		_T("html {font-family: sans-serif;}") +
		_T("body {padding: 0 1em 1em;}") +
		_T("nav, div.wikistyle, #comments{   width:760px;  margin: 0 auto;}") +
		_T("div.wikistyle, #comments {  margin-top: 1em;}") +
		_T("nav a {  margin-right: 0.5em;}") +
		_T("nav, div.wikistyle, #comments{background-color:#f8f8f8;padding:.7em;}") +
		_T("nav, div.wikistyle, #comments{border:1px solid #e9e9e9;}") +
		_T("h1,h2,h3,h4,h5,h6{border:0!important;}") +
		_T("h1{font-size:170%!important;border-top:4px solid #aaa!important;padding-top:.5em!important;margin-top:1.5em!important;}") +
		_T("h1:first-child{margin-top:0!important;padding-top:.25em!important;border-top:none!important;}") +
		_T("h2{font-size:150%!important;margin-top:1.5em!important;border-top:4px solid #e0e0e0!important;padding-top:.5em!important;}") +
		_T("h3{margin-top:1em!important;}") +
		_T("p{margin:1em 0!important;line-height:1.5em!important;}") +
		_T("a.absent{color:#a00;}") +
		_T("ul,#wiki-form .content-body ul{margin:1em 0 1em 0em!important;}") +
		_T("ol,#wiki-form .content-body ol{margin:1em 0 1em 0em!important;}") +
		_T("ul li,#wiki-form .content-body ul li,ol li,#wiki-form .content-body ol li{margin-top:.5em;margin-bottom:.5em;}") +
		_T("ul ul,ul ol,ol ol,ol ul,#wiki-form .content-body ul ul,#wiki-form .content-body ul ol,#wiki-form .content-body ol ol,#wiki-form .content-body ol ul{margin-top:0!important;margin-bottom:0!important;}") +
		_T("blockquote{margin:1em 0!important;border-left:5px solid #ddd!important;padding-left:.6em!important;color:#555!important;}") +
		_T("dt{font-weight:bold!important;margin-left:1em!important;}") +
		_T("dd{margin-left:2em!important;margin-bottom:1em!important;}") +
		_T("table{margin:1em 0!important;}") +
		_T("table thead{margin:1em 0!important;background-color: gray!important;}") +
		_T("table th{border-bottom:1px solid #bbb!important;padding:.2em 1em!important;}") +
		_T("table td{border-bottom:1px solid #ddd!important;padding:.2em 1em!important;}") +
		_T("pre{margin:1em 0;font-size:12px;background-color:#eee;border:1px solid #ddd;padding:5px;line-height:1.5em;color:#444;overflow:auto;-webkit-box-shadow:rgba(0,0,0,0.07) 0 1px 2px inset;-webkit-border-radius:3px;-moz-border-radius:3px;border-radius:3px;}") +
		_T("pre::-webkit-scrollbar{height:8px;width:8px;}") +
		_T("pre::-webkit-scrollbar-track-piece{margin-bottom:10px;background-color:#e5e5e5;border-bottom-left-radius:4px 4px;border-bottom-right-radius:4px 4px;border-top-left-radius:4px 4px;border-top-right-radius:4px 4px;}") +
		_T("pre::-webkit-scrollbar-thumb:vertical{height:25px;background-color:#ccc;-webkit-border-radius:4px;-webkit-box-shadow:0 1px 1px rgba(255,255,255,1);}") +
		_T("pre::-webkit-scrollbar-thumb:horizontal{width:25px;background-color:#ccc;-webkit-border-radius:4px;}") +
		_T("pre code{padding:0!important;font-size:12px!important;background-color:#eee!important;border:none!important;}") +
		_T("code{font-size:14px!important;background-color:#f8f8ff!important;color:#444!important;padding:0 .2em!important;border:1px solid #dedede!important;}") +
		_T("a code,a:link code,a:visited code{color:#4183c4!important;}") +
		_T("img{max-width:100%;}") +
		_T("pre.console{margin:1em 0!important;font-size:12px!important;background-color:black!important;padding:.5em!important;line-height:1.5em!important;color:white!important;}") +
		_T("pre.console code{padding:0!important;font-size:12px!important;background-color:black!important;border:none!important;color:white!important;}") +
		_T("pre.console span{color:#888!important;}") +
		_T("pre.console span.command{color:yellow!important;}") +
		_T(".frame{margin:0;display:inline-block;}") +
		_T(".frame img{display:block;}") +
		_T(".frame>span{display:block;border:1px solid #aaa;padding:4px;}") +
		_T(".frame span span{display:block;font-size:10pt;margin:0;padding:4px 0 2px 0;text-align:center;line-height:10pt;font-family:'Helvetica Neue',Helvetica,Arial,sans-serif;}") +
		_T(".float-left{float:left;padding:.5em 1em .25em 0;}") +
		_T(".float-right{float:right;padding:.5em 0 .25em 1em;}") +
		_T(".align-left{display:block;text-align:left;}") +
		_T(".align-center{display:block;text-align:center;}") +
		_T(".align-right{display:block;text-align:right;}");

		createDefaultCSSFile(m_cssPath, cssData);
	}
}

// CmdwEditorDoc commands

void CmdwEditorDoc::OnFileExport()
{
	// TODO: Add your command handler code here
	// coopy m_htmlPath to new file.

	if(!m_bHtmlExisted) {
		AfxMessageBox(_T("Please open a .md file or save current new .md file\nbefore exporting html."));
		return;
	}

	static TCHAR BASED_CODE szFilter[] = _T("Html Files (*.html)|*.html|All Files(*.*)|*.*||");
	CFileDialog fdlg(FALSE, _T("html"), _T("*.html"), OFN_PATHMUSTEXIST, szFilter);	

	if(fdlg.DoModal() == IDOK) {
		if(m_bHtmlExisted && (fdlg.GetPathName().IsEmpty() == false) )
			::CopyFile(m_htmlPath, fdlg.GetPathName(), FALSE);
	}
}
