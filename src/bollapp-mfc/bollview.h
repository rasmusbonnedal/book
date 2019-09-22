#pragma once
#include <afxwin.h>
#include <afxcmn.h>

class BollView : public CView {
protected:
	BollView();
	DECLARE_DYNCREATE(BollView)

	DECLARE_MESSAGE_MAP()

	CListCtrl m_mainList;
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnDraw(CDC* pDC);
};
