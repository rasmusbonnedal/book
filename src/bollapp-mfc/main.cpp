#include <afx.h>

#include "Resource.h"
#include "bollframe.h"
#include "bollview.h"
#include "mfcbolldoc.h"

class BollApp : public CWinApp {
public:
    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(BollApp, CWinApp)
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()

BollApp theApp;

BOOL BollApp::InitInstance() {
    CWinApp::InitInstance();

    CSingleDocTemplate* pDocTemplate = new CSingleDocTemplate(IDR_MAINFRAME,
        RUNTIME_CLASS(MFCBollDoc),
        RUNTIME_CLASS(BollFrame),
        RUNTIME_CLASS(BollView));

    if (!pDocTemplate) {
        return FALSE;
    }
    AddDocTemplate(pDocTemplate);

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    // Enable DDE Execute open
    EnableShellOpen();
    RegisterShellFileTypes(TRUE);

    // Dispatch commands specified on the command line.  Will return FALSE if
    // app was launched with /RegServer, /Register, /Unregserver or /Unregister.
    if (!ProcessShellCommand(cmdInfo)) {
        return FALSE;
    }

    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();
    return TRUE;
}
