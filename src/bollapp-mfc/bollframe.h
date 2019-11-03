#pragma once
#include <afxext.h>
#include <afxwin.h>

class BollFrame : public CFrameWnd {
protected:
    BollFrame();
    DECLARE_DYNCREATE(BollFrame)

    CStatusBar m_wndStatusBar;

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};
