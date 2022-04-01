#include "pch.h"
#include "framework.h"
#include "WebsocketDebugger.h"
#include "WebsocketDebuggerDlg.h"
#include "afxdialogex.h"

#include "DialogNewCustomHeader.h"
#include "Websocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


CWebsocketDebuggerDlg::CWebsocketDebuggerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WEBSOCKETDEBUGGER_DIALOG, pParent)
	, mEditValueServerURL(_T(""))
	, mEditValueServerSubprotocol(_T(""))
	, mCheckValueAllowSelfSigned(FALSE)
	, mEditValueLog(_T(""))
	, mEditValueSend(_T(""))
	, mEditValueLogMaxLine (500)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWebsocketDebuggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange (pDX);
	DDX_Control (pDX, IDC_EDIT_SERVER_URL, mEditControlServerURL);
	DDX_Text (pDX, IDC_EDIT_SERVER_URL, mEditValueServerURL);
	DDX_Control (pDX, IDC_EDIT_SERVER_SUBPROTOCOL, mEditControlServerSubprotocol);
	DDX_Text (pDX, IDC_EDIT_SERVER_SUBPROTOCOL, mEditValueServerSubprotocol);
	DDX_Control (pDX, IDC_CHECK_ALLOW_SELFSIGNED, mCheckControlAllowSelfSigned);
	DDX_Check (pDX, IDC_CHECK_ALLOW_SELFSIGNED, mCheckValueAllowSelfSigned);
	DDX_Control (pDX, IDC_LIST_CUSTOMHEADERS, mListControlCustomHeaders);
	DDX_Control (pDX, IDC_EDIT_LOG, mEditControlLog);
	DDX_Text (pDX, IDC_EDIT_LOG, mEditValueLog);
	DDX_Control (pDX, IDC_EDIT_SEND, mEditControlSend);
	DDX_Text (pDX, IDC_EDIT_SEND, mEditValueSend);
	DDX_Control (pDX, IDOK, mButtonControlOk);
	DDX_Control (pDX, IDC_BUTTON_CUSTOMHEADERS_ADD, mButtonControlCustomHeadersAdd);
	DDX_Control (pDX, IDC_BUTTON_CUSTOMHEADERS_REMOVE, mButtonControlCustomHeadersRemove);
	DDX_Control (pDX, IDC_EDIT_LOG_MAX_LINE, mEditControlLogMaxLine);
	DDX_Text (pDX, IDC_EDIT_LOG_MAX_LINE, mEditValueLogMaxLine);
}

BEGIN_MESSAGE_MAP(CWebsocketDebuggerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED (IDC_BUTTON_CUSTOMHEADERS_ADD, &CWebsocketDebuggerDlg::OnBnClickedButtonCustomheadersAdd)
	ON_BN_CLICKED (IDC_BUTTON_CUSTOMHEADERS_REMOVE, &CWebsocketDebuggerDlg::OnBnClickedButtonCustomheadersRemove)
	ON_BN_CLICKED (IDOK, &CWebsocketDebuggerDlg::OnBnClickedOk)
	ON_MESSAGE (WM_PUT_THREAD_MESSAGE, OnPutThreadMessage)
	ON_WM_DESTROY ()
	ON_NOTIFY (NM_DBLCLK, IDC_LIST_CUSTOMHEADERS, &CWebsocketDebuggerDlg::OnNMDblclkListCustomheaders)
END_MESSAGE_MAP()

BOOL CWebsocketDebuggerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	//////////////////////////////////////////////////
	initCustomHeadersControl ();
	mEditControlLog.SetScrollRange (SB_VERT, 0, INT_MAX);

	mWebsocket = new CWebsocket (this);

	initRegistry ();

#if defined (_DEBUG)
	SetWindowText (_T ("WebSocket Debugger (DEBUG Mode)"));
#else
	SetWindowText (_T ("WebSocket Debugger"));
#endif

	setControls ();

	return TRUE;
}

void CWebsocketDebuggerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CWebsocketDebuggerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

BOOL CWebsocketDebuggerDlg::PreTranslateMessage (MSG *pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		if (this->GetFocus ()->GetDlgCtrlID () == IDC_EDIT_SEND) {
			sendMessage ();
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage (pMsg);
}

HCURSOR CWebsocketDebuggerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWebsocketDebuggerDlg::OnDestroy ()
{
	CDialogEx::OnDestroy ();

	if (mWebsocket) {
		if (mWebsocket->getIsRun ())
			mWebsocket->close ();
		stopThread ();
		delete mWebsocket;
	}
}

void CWebsocketDebuggerDlg::initCustomHeadersControl (void)
{
	LV_COLUMN _column;
	_column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	_column.fmt = LVCFMT_LEFT;

	mListControlCustomHeaders.SetExtendedStyle (LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	_column.iSubItem = 0;
	_column.pszText = _T ("Key");
	_column.cx = 150;
	mListControlCustomHeaders.InsertColumn (_column.iSubItem, &_column);

	_column.iSubItem = 1;
	_column.pszText = _T ("Value");
	_column.cx = 240;
	mListControlCustomHeaders.InsertColumn (_column.iSubItem, &_column);
}

void CWebsocketDebuggerDlg::OnBnClickedButtonCustomheadersAdd ()
{
	CDialogNewCustomHeader _dlg;
	if (_dlg.DoModal () == IDOK) {
		LV_ITEM _item;
		_item.mask = LVIF_TEXT;
		_item.iItem = mListControlCustomHeaders.GetItemCount ();
		_item.iSubItem = 0;
		_item.pszText = _dlg.mEditValueCHKey.GetBuffer ();
		mListControlCustomHeaders.InsertItem (&_item);

		_item.iSubItem = 1;
		_item.pszText = _dlg.mEditValueCHValue.GetBuffer ();
		mListControlCustomHeaders.SetItem (&_item);

		mListControlCustomHeaders.SendMessage (WM_VSCROLL, SB_BOTTOM);
	}
}

void CWebsocketDebuggerDlg::OnBnClickedButtonCustomheadersRemove ()
{
	int i;
	for (i = mListControlCustomHeaders.GetItemCount () - 1; i >= 0; i--) {
		if (mListControlCustomHeaders.GetItemState (i, LVIS_SELECTED) & LVIS_SELECTED)
			mListControlCustomHeaders.DeleteItem (i);
	}
}

void CWebsocketDebuggerDlg::OnNMDblclkListCustomheaders (NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	if (pNMItemActivate->iItem >= 0) {
		CDialogNewCustomHeader _dlg;
		_dlg.SetModifyData (mListControlCustomHeaders.GetItemText (pNMItemActivate->iItem, 0), mListControlCustomHeaders.GetItemText (pNMItemActivate->iItem, 1));
		if (_dlg.DoModal () == IDOK) {
			mListControlCustomHeaders.SetItemText (pNMItemActivate->iItem, 0, _dlg.mEditValueCHKey.GetBuffer ());
			mListControlCustomHeaders.SetItemText (pNMItemActivate->iItem, 1, _dlg.mEditValueCHValue.GetBuffer ());
		}
	}

	*pResult = 0;
}

void CWebsocketDebuggerDlg::OnBnClickedOk ()
{
	UpdateData (true);

	if (mWebsocket->getIsRun () == false) {
		saveRegistry ();

		mWebsocket->setIsRun (true);

		setControls ();

		mWebsocket->setURL (mEditValueServerURL);
		mWebsocket->setSubprotocol (mEditValueServerSubprotocol);
		mWebsocket->setSelfSigned (mCheckValueAllowSelfSigned);
		mWebsocket->clearCustomHeaders ();
		for (int i = 0; i < mListControlCustomHeaders.GetItemCount (); i++)
			mWebsocket->addCustomHeaders (mListControlCustomHeaders.GetItemText (i, 0), mListControlCustomHeaders.GetItemText (i, 1));

		try {
			mThreadWebSocket = std::thread (&CWebsocket::run, mWebsocket);
		}
		catch (const std::exception &e) {
			std::string _message = "[ERROR] Thread start failed : " + std::string (e.what ()) + "\n";
			mWebsocket->postLogMessageANSI (_message);
#if defined (_DEBUG)
			OutputDebugStringA (_message.c_str ());
#endif
		}
	}
	else {
		mWebsocket->close ();
	}

	//CDialogEx::OnOK ();
}

LRESULT CWebsocketDebuggerDlg::OnPutThreadMessage (WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
		case POSTMESSAGE_TYPE_RESET_CONTROLS:
			setControls ();
			break;
		case POSTMESSAGE_TYPE_STOP_WSTHREAD:
			stopThread ();
			break;
		case POSTMESSAGE_TYPE_UTF8: // run setLog () / utf8
		case POSTMESSAGE_TYPE_ANSI: // run setLog () / ansi
			if (lParam == 0)
				break;
			char *_str = (char *) lParam;
			setLog ((wParam == POSTMESSAGE_TYPE_UTF8) ? UTF8_TO_CSTRING (_str) : CString (_str));
			delete _str;
			break;
	}
	return 0;
}

void CWebsocketDebuggerDlg::setLog (CString &message)
{
	UpdateData (true);

	CTime _time = CTime::GetCurrentTime ();

	message.Trim ();
	message.Replace (_T ("\r\n"), _T ("\\n"));
	message.Replace (_T ("\n"), _T ("\\n"));

	if (mEditValueLog.GetLength () > 0)
		mEditValueLog.Append (_T ("\r\n"));
	mEditValueLog.AppendFormat (_T ("%04d/%02d/%02d %02d:%02d:%02d "), _time.GetYear (), _time.GetMonth (), _time.GetDay (), _time.GetHour (), _time.GetMinute (), _time.GetSecond ());
	mEditValueLog.Append (message);

	// 개행 개수
	CString _newline = _T ("\r\n");
	int _count, _index;
	for (_count = 0, _index = 0; _index < mEditValueLog.GetLength (); _count++) {
		if ((_index = mEditValueLog.Find (_newline, _index)) < 0)
			break;
		_index += _newline.GetLength ();
	}

	if (_count > (mEditValueLogMaxLine - 1)) {
		_index = mEditValueLog.Find (_newline, 0);
		mEditValueLog.Delete (0, _index + _newline.GetLength ());
		_count--;
	}

	UpdateData (false);

	// after UpdateData
	mEditControlLog.LineScroll (mEditControlLog.GetLineCount ());
}

void CWebsocketDebuggerDlg::setControls (void)
{
	mCheckControlAllowSelfSigned.SetCheck (mWebsocket->getSelfSigned () == true);

	mEditControlServerURL.EnableWindow (mWebsocket->getIsRun () == false);
	mEditControlServerSubprotocol.EnableWindow (mWebsocket->getIsRun () == false);
	mCheckControlAllowSelfSigned.EnableWindow (mWebsocket->getIsRun () == false);
	mListControlCustomHeaders.EnableWindow (mWebsocket->getIsRun () == false);
	mButtonControlCustomHeadersAdd.EnableWindow (mWebsocket->getIsRun () == false);
	mButtonControlCustomHeadersRemove.EnableWindow (mWebsocket->getIsRun () == false);
	mEditControlSend.EnableWindow (mWebsocket->getIsRun () == true);
	mEditControlLogMaxLine.EnableWindow (mWebsocket->getIsRun () == false);
	mButtonControlOk.SetWindowText ((mWebsocket->getIsRun () == true) ? _T ("STOP") : _T ("START"));

	UpdateData (false);

	mEditControlLog.LineScroll (mEditControlLog.GetLineCount ());
}

void CWebsocketDebuggerDlg::sendMessage (void)
{
	UpdateData (true);
	mWebsocket->sendMessage (mEditValueSend);
	mEditValueSend.Empty ();
	UpdateData (false);
}

void CWebsocketDebuggerDlg::stopThread (void)
{
	mWebsocket->stop ();

	try {
		if (mThreadWebSocket.joinable ())
			mThreadWebSocket.join ();
	}
	catch (const std::exception &e) {
		std::string _message = "[ERROR] Thread join failed : " + std::string (e.what ()) + "\n";

		mWebsocket->postLogMessage (POSTMESSAGE_TYPE_ANSI, _message);
#if defined (_DEBUG)
		OutputDebugStringA (_message.c_str ());
#endif
	}
}

void CWebsocketDebuggerDlg::initRegistry (void)
{
	if (RegOpenKey (HKEY_CURRENT_USER, REG_SUBKEY, &mRegKey) != ERROR_SUCCESS)
		RegCreateKey (HKEY_CURRENT_USER, REG_SUBKEY, &mRegKey);

	DWORD _dwtype;
	DWORD _cbdata;
	WCHAR _wchar[0xfff];
	int _customheaders_count;
	if (RegGetValue (HKEY_CURRENT_USER, REG_SUBKEY, REG_KEY_URL, RRF_RT_REG_SZ, &_dwtype, _wchar, &(_cbdata = sizeof (WCHAR) * 0xfff)) == ERROR_SUCCESS)
		mEditValueServerURL = _wchar;
	if (RegGetValue (HKEY_CURRENT_USER, REG_SUBKEY, REG_KEY_SUBPROTOCOL, RRF_RT_REG_SZ, &_dwtype, _wchar, &(_cbdata = sizeof (WCHAR) * 0xfff)) == ERROR_SUCCESS)
		mEditValueServerSubprotocol = _wchar;
	RegGetValue (HKEY_CURRENT_USER, REG_SUBKEY, REG_KEY_SELF_SIGNED, RRF_RT_DWORD, &_dwtype, &mCheckValueAllowSelfSigned, &(_cbdata = sizeof (mCheckValueAllowSelfSigned)));
	RegGetValue (HKEY_CURRENT_USER, REG_SUBKEY, REG_KEY_CUSTOMHEADERS_COUNT, RRF_RT_DWORD, &_dwtype, &_customheaders_count, &(_cbdata = sizeof (_customheaders_count)));
	CString _item;
	for (int i = 0; i < _customheaders_count; i++) {
		_item.Format (_T ("%s%d"), REG_KEY_CUSTOMHEADERS_KEY, i);
		if (RegGetValue (HKEY_CURRENT_USER, REG_SUBKEY, _item, RRF_RT_REG_SZ, &_dwtype, _wchar, &(_cbdata = sizeof (WCHAR) * 0xfff)) == ERROR_SUCCESS) {
			mListControlCustomHeaders.InsertItem (i, _wchar);
			_item.Format (_T ("%s%d"), REG_KEY_CUSTOMHEADERS_VAL, i);
			if (RegGetValue (HKEY_CURRENT_USER, REG_SUBKEY, _item, RRF_RT_REG_SZ, &_dwtype, _wchar, &(_cbdata = sizeof (WCHAR) * 0xfff)) == ERROR_SUCCESS)
				mListControlCustomHeaders.SetItemText (i, 1, _wchar);
		}
	}
	RegGetValue (HKEY_CURRENT_USER, REG_SUBKEY, REG_KEY_LOG_MAX_LINES, RRF_RT_DWORD, &_dwtype, &mEditValueLogMaxLine, &(_cbdata = sizeof (mEditValueLogMaxLine)));
	RegCloseKey (mRegKey);

}
void CWebsocketDebuggerDlg::saveRegistry (void)
{
	if (RegOpenKey (HKEY_CURRENT_USER, REG_SUBKEY, &mRegKey) == ERROR_SUCCESS) {
		CString _item_key, _item_value;
		int _customheaders_count = mListControlCustomHeaders.GetItemCount ();
		RegSetValueEx (mRegKey, REG_KEY_URL, 0, REG_SZ, (const BYTE *) mEditValueServerURL.GetBuffer (), sizeof (WCHAR) * (mEditValueServerURL.GetLength () + 1));
		RegSetValueEx (mRegKey, REG_KEY_SUBPROTOCOL, 0, REG_SZ, (const BYTE *) mEditValueServerSubprotocol.GetBuffer (), sizeof (WCHAR) * (mEditValueServerSubprotocol.GetLength () + 1));
		RegSetValueEx (mRegKey, REG_KEY_SELF_SIGNED, 0, REG_DWORD, (const BYTE *) &mCheckValueAllowSelfSigned, sizeof (mCheckValueAllowSelfSigned));
		RegSetValueEx (mRegKey, REG_KEY_CUSTOMHEADERS_COUNT, 0, REG_DWORD, (const BYTE *) &_customheaders_count, sizeof (_customheaders_count));
		for (int i = 0; i < _customheaders_count; i++) {
			_item_key.Format (_T ("%s%d"), REG_KEY_CUSTOMHEADERS_KEY, i);
			_item_value = mListControlCustomHeaders.GetItemText (i, 0);
			RegSetValueEx (mRegKey, _item_key, 0, REG_SZ, (const BYTE *) _item_value.GetBuffer (), sizeof (WCHAR) * (_item_value.GetLength () + 1));
			_item_key.Format (_T ("%s%d"), REG_KEY_CUSTOMHEADERS_VAL, i);
			_item_value = mListControlCustomHeaders.GetItemText (i, 1);
			RegSetValueEx (mRegKey, _item_key, 0, REG_SZ, (const BYTE *) _item_value.GetBuffer (), sizeof (WCHAR) * (_item_value.GetLength () + 1));
		}
		RegSetValueEx (mRegKey, REG_KEY_LOG_MAX_LINES, 0, REG_DWORD, (const BYTE *) &mEditValueLogMaxLine, sizeof (mEditValueLogMaxLine));
		RegCloseKey (mRegKey);
	}
}
