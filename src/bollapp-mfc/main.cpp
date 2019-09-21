#include <afxwin.h>
#include <afxext.h>
#include "Resource.h"

class BollApp : public CWinApp {
public:
    virtual BOOL InitInstance();
};

class BollFrame : public CFrameWnd {
protected:
    BollFrame();
	DECLARE_DYNCREATE(BollFrame)

	CStatusBar m_wndStatusBar;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};

class BollDoc : public CDocument {
protected:
	BollDoc();
	DECLARE_DYNCREATE(BollDoc)

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnNewDocument();
};

class BollView : public CScrollView {
protected:
	BollView();
	DECLARE_DYNCREATE(BollView)

	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitialUpdate();
	virtual void OnDraw(CDC* pDC);
};

BollApp theApp;

BOOL BollApp::InitInstance() {
	CWinApp::InitInstance();

	CSingleDocTemplate* pDocTemplate = new CSingleDocTemplate(IDR_MAINFRAME,
		RUNTIME_CLASS(BollDoc),
		RUNTIME_CLASS(BollFrame),
		RUNTIME_CLASS(BollView));

	if (!pDocTemplate) {
		return FALSE;
	}
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo)) {
		return FALSE;
	}

    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();
    return TRUE;
}

// --- BollFrame ---
IMPLEMENT_DYNCREATE(BollFrame, CFrameWnd);

BEGIN_MESSAGE_MAP(BollFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
    ON_WM_PAINT()
END_MESSAGE_MAP()

BollFrame::BollFrame() {
}

BOOL BollFrame::PreCreateWindow(CREATESTRUCT& cs) {
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

namespace {
	UINT indicators[] = {
		ID_SEPARATOR,           // status line indicator
		ID_INDICATOR_CAPS,
		ID_INDICATOR_NUM,
		ID_INDICATOR_SCRL,
	};
}

int BollFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	return 0;
}

void BollFrame::OnLButtonDown(UINT nFlags, CPoint point) {
    TRACE("Entering BollFrame::OnLButtonDown - %lx, %d, %d\n", (long)nFlags, point.x, point.y);
}

void BollFrame::OnPaint() {
    CPaintDC dc(this);
    dc.TextOut(0, 0, "Hello World!");
}

// --- BollDoc ---
IMPLEMENT_DYNCREATE(BollDoc, CDocument);

BEGIN_MESSAGE_MAP(BollDoc, CDocument)
END_MESSAGE_MAP()

BollDoc::BollDoc() {
}

BOOL BollDoc::OnNewDocument() {
	if (!CDocument::OnNewDocument()) {
		return FALSE;
	}

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}


// --- BollView ---
IMPLEMENT_DYNCREATE(BollView, CScrollView);

BEGIN_MESSAGE_MAP(BollView, CScrollView)
END_MESSAGE_MAP()

BollView::BollView() {
}

void BollView::OnInitialUpdate() {
	SetScrollSizes(MM_TEXT, { 100, 100 });
}

void BollView::OnDraw(CDC* /*pDC*/)
{
	SetScrollSizes(MM_TEXT, { 100, 100 });
	CDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) {
		return;
	}

	// TODO: add draw code for native data here
}
