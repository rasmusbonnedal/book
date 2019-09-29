#pragma once
#include <afxwin.h>
#include <afxcmn.h>

class MFCBollDoc;

class BollView : public CView {
protected:
    BollView();
    DECLARE_DYNCREATE(BollView)

    DECLARE_MESSAGE_MAP()

    CListCtrl m_mainList;

    MFCBollDoc* GetDocument();

    afx_msg void OnSize(UINT nType, int cx, int cy);
public:
    virtual void OnDestroy();
    virtual void OnInitialUpdate();
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    virtual void OnDraw(CDC* pDC);
};
