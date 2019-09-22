#include "mfcbolldoc.h"

IMPLEMENT_DYNCREATE(MFCBollDoc, CDocument);

BEGIN_MESSAGE_MAP(MFCBollDoc, CDocument)
END_MESSAGE_MAP()

MFCBollDoc::MFCBollDoc() {
}

BOOL MFCBollDoc::OnNewDocument() {
	if (!CDocument::OnNewDocument()) {
		return FALSE;
	}

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}
