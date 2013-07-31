
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
END_MESSAGE_MAP()



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

void convMd2Html(LPCTSTR outpath)
{
	struct buf *ob;
	int ret;
	FILE *outf = NULL;
	unsigned int extensions = 0xff;
	char hHead[] = "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=""utf-8"">\n";
	char hCSS[] = "<link href=""ex.css"" rel=""stylesheet""></link>\n</head>\n<body>\n";
	char hTail[] = "</body>\n</html>";

	struct sd_callbacks callbacks;
	struct html_renderopt options;
	struct sd_markdown *markdown;

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
		fwrite(hCSS, 1, strlen(hHead), outf);
		/* writing the result to stdout */
		ret = fwrite(ob->data, 1, ob->size, outf);
		fwrite(hTail, 1, strlen(hTail), outf);
		fclose(outf);
	}

	/* cleanup */
	bufrelease(mib_utf8);
	bufrelease(ob);
	mib_utf8 = NULL;
}

// CmdwEditorDoc construction/destruction

CmdwEditorDoc::CmdwEditorDoc()
{
	// TODO: add one-time construction code here

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

	CString htmlpath;
	CFile *pCF = ar.GetFile();
	if(pCF != NULL)
		htmlpath = pCF->GetFilePath() + _T(".html");

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

			convMd2Html((LPCTSTR)htmlpath);
			this->UpdateAllViews(NULL, (LPARAM)(LPCTSTR)htmlpath, NULL);
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
		convMd2Html((LPCTSTR)htmlpath);
		this->UpdateAllViews(NULL, (LPARAM)(LPCTSTR)htmlpath, NULL);
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


// CmdwEditorDoc commands
