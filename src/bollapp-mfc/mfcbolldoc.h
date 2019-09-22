#pragma once

#include <afx.h>
#include <afxwin.h>

#include <memory>

class BollDoc;

class MFCBollDoc : public CDocument {
protected:
    MFCBollDoc();
    DECLARE_DYNCREATE(MFCBollDoc)

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);

    BollDoc& getDocument();

private:
    std::unique_ptr<BollDoc> m_bolldoc;
};
