#include "bollview.h"

#include <sstream>

IMPLEMENT_DYNCREATE(BollView, CView);

BEGIN_MESSAGE_MAP(BollView, CView)
END_MESSAGE_MAP()

BollView::BollView() {
}

BOOL BollView::PreCreateWindow(CREATESTRUCT& cs) {
	return CView::PreCreateWindow(cs);
}

void BollView::OnInitialUpdate() {
	BOOL result = m_mainList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL, { 100, 100, 800, 600 }, this, 1001);
	if (!result) {
		TRACE("Error could not create CListCtrl");
	}
	m_mainList.SetExtendedStyle(m_mainList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_AUTOSIZECOLUMNS | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_mainList.InsertColumn(0, "Verifikat");
	m_mainList.InsertColumn(1, "Datum");
	m_mainList.InsertColumn(2, "Text");
	m_mainList.InsertColumn(3, "Omslutning");

	CView::OnInitialUpdate();
}

void BollView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) {
	m_mainList.DeleteAllItems();

	for (int i = 0; i < 500; ++i) {
		std::stringstream ss;
		ss << i;
		m_mainList.InsertItem(i, ss.str().c_str());
		m_mainList.SetItemText(i, 1, "2019-09-22");
		m_mainList.SetItemText(i, 2, "Köpa grejer i Sisjön");
		m_mainList.SetItemText(i, 3, "5000");
	}
	m_mainList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_mainList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	m_mainList.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
	m_mainList.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);
	CView::OnUpdate(pSender, lHint, pHint);
}

void BollView::OnDraw(CDC* /*pDC*/)
{
	CDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) {
		return;
	}
}
