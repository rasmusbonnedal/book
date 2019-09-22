#pragma once

#include <afx.h>
#include <afxwin.h>

class MFCBollDoc : public CDocument {
protected:
	MFCBollDoc();
	DECLARE_DYNCREATE(MFCBollDoc)

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnNewDocument();
};
