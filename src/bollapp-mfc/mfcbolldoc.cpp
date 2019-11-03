#include "mfcbolldoc.h"

#include "bolldoc.h"
#include "mfcstring.h"
#include "serialize.h"

#include <fstream>
#include <memory>
#include <sstream>

IMPLEMENT_DYNCREATE(MFCBollDoc, CDocument);

BEGIN_MESSAGE_MAP(MFCBollDoc, CDocument)
END_MESSAGE_MAP()

MFCBollDoc::MFCBollDoc() {}

BollDoc& MFCBollDoc::getDocument() { return *m_bolldoc; }

BOOL MFCBollDoc::OnNewDocument() {
    if (!CDocument::OnNewDocument()) {
        return FALSE;
    }

    m_bolldoc = std::make_unique<BollDoc>(2074, "Firma", "556677-1234", 2019,
                                          "SEK", false);

    return TRUE;
}

void MFCBollDoc::Serialize(CArchive& ar) {
    if (ar.IsStoring()) {
        std::stringstream ss;
        Serialize::saveDocument(*m_bolldoc, ss);
        ar.Write(ss.str().c_str(), static_cast<UINT>(ss.str().size()));
    } else {
        std::string s;
        char buf[1024];
        UINT readBytes;
        do {
            readBytes = ar.Read(buf, sizeof(buf));
            s.append(buf, readBytes);
        } while (readBytes == sizeof(buf));

        std::stringstream ss;
        ss.str(s);
        try {
            m_bolldoc = std::make_unique<BollDoc>(
                std::move(Serialize::loadDocument(ss)));
        } catch (std::exception& e) {
            AfxMessageBox(fromUtf8(e.what()));
            AfxThrowArchiveException(CArchiveException::badIndex,
                                     ar.GetFile()->GetFileName());
        }
    }
}
