#include "bollview.h"

#include "mfcbolldoc.h"
#include "mfcstring.h"

#include "bolldoc.h"

#include <sstream>

IMPLEMENT_DYNCREATE(BollView, CView);

BEGIN_MESSAGE_MAP(BollView, CView)
END_MESSAGE_MAP()

BollView::BollView() {
}

MFCBollDoc* BollView::GetDocument() {
    return dynamic_cast<MFCBollDoc*>(m_pDocument);
}

void BollView::OnInitialUpdate() {
    if (m_mainList.GetSafeHwnd() != 0) {
        AfxMessageBox(L"Already created m_mainList control");
    }
    else {
        BOOL result = m_mainList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL, { 100, 100, 800, 600 }, this, 1001);
        if (!result) {
            TRACE("Error could not create CListCtrl");
        }
        m_mainList.SetExtendedStyle(m_mainList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_AUTOSIZECOLUMNS | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
        m_mainList.InsertColumn(0, L"Verifikat");
        m_mainList.InsertColumn(1, L"Datum");
        m_mainList.InsertColumn(2, L"Text");
        m_mainList.InsertColumn(3, L"Omslutning");
    }
    CView::OnInitialUpdate();
}

void BollView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) {
    m_mainList.DeleteAllItems();
    MFCBollDoc* doc = GetDocument();
    BollDoc& bolldoc = doc->getDocument();

    for (auto& v : bolldoc.getVerifikationer()) {
        m_mainList.InsertItem(v.getUnid(), toCString(v.getUnid()));
        m_mainList.SetItemText(v.getUnid(), 1, toCString(v.getTransdatum()));
        m_mainList.SetItemText(v.getUnid(), 2, fromUtf8(v.getText()));
        Pengar omslutning;
        CString omslutningString;
        if (v.getOmslutning(omslutning)) {
            omslutningString = toCString(omslutning);
        } else {
            omslutningString = "Obalanserad";
        }
        m_mainList.SetItemText(v.getUnid(), 3, omslutningString);
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
