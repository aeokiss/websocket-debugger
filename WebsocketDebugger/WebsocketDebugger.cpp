#include "pch.h"
#include "framework.h"
#include "WebsocketDebugger.h"
#include "WebsocketDebuggerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWebsocketDebuggerApp
BEGIN_MESSAGE_MAP(CWebsocketDebuggerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CWebsocketDebuggerApp::CWebsocketDebuggerApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

CWebsocketDebuggerApp theApp;

BOOL CWebsocketDebuggerApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	CShellManager *pShellManager = new CShellManager;

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CWebsocketDebuggerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// 
	}
	else if (nResponse == IDCANCEL)
	{
		// 
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: Dialog creation failed, the application will terminate unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: If you use MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	return FALSE;
}

