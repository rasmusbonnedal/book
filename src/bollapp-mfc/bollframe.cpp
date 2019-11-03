#include "bollframe.h"

IMPLEMENT_DYNCREATE(BollFrame, CFrameWnd);

BEGIN_MESSAGE_MAP(BollFrame, CFrameWnd)
ON_WM_CREATE()
ON_WM_LBUTTONDOWN()
ON_WM_PAINT()
END_MESSAGE_MAP()

BollFrame::BollFrame() {}

BOOL BollFrame::PreCreateWindow(CREATESTRUCT& cs) {
    if (!CFrameWnd::PreCreateWindow(cs)) {
        return FALSE;
    }
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return TRUE;
}

namespace {
UINT indicators[] = {
    ID_SEPARATOR, // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};
}

int BollFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    if (!m_wndStatusBar.Create(this)) {
        TRACE0("Failed to create status bar\n");
        return -1; // fail to create
    }
    m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
    return 0;
}
