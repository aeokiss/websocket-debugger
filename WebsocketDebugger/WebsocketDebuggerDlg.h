#pragma once

#include <thread>

#define REG_SUBKEY					_T("SOFTWARE\\aeokiss\\WebsocketDebugger")
#define REG_KEY_URL					_T("URL")
#define REG_KEY_SUBPROTOCOL			_T("SUBPROTOCOL")
#define REG_KEY_SELF_SIGNED			_T("SELF_SIGNED")
#define REG_KEY_CUSTOMHEADERS_COUNT	_T("CUSTOMHEADERS_COUNT")
#define REG_KEY_CUSTOMHEADERS_KEY	_T("CUSTOMHEADERS_KEY_")
#define REG_KEY_CUSTOMHEADERS_VAL	_T("CUSTOMHEADERS_VAL_")
#define REG_KEY_LOG_MAX_LINES		_T("LOG_MAX_LINES")

class CWebsocket;

class CWebsocketDebuggerDlg : public CDialogEx
{
public:
	CWebsocketDebuggerDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WEBSOCKETDEBUGGER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage (MSG *pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnPutThreadMessage (WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonCustomheadersAdd ();
	afx_msg void OnBnClickedButtonCustomheadersRemove ();
	afx_msg void OnNMDblclkListCustomheaders (NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk ();
	afx_msg void OnDestroy ();
	DECLARE_MESSAGE_MAP()

private:
	void initCustomHeadersControl (void);
	void setLog (CString &message);
	void setControls (void);
	void sendMessage (void);
	void stopThread (void);
	void initRegistry (void);
	void saveRegistry (void);

	HKEY mRegKey;
	std::thread mThreadWebSocket;
	CWebsocket *mWebsocket;

public:
	CEdit mEditControlServerURL;
	CEdit mEditControlServerSubprotocol;
	CButton mCheckControlAllowSelfSigned;
	CListCtrl mListControlCustomHeaders;
	CEdit mEditControlLog;
	CButton mButtonControlCustomHeadersAdd;
	CButton mButtonControlCustomHeadersRemove;
	CEdit mEditControlSend;
	CButton mButtonControlOk;
	CEdit mEditControlLogMaxLine;

	CString mEditValueServerURL;
	CString mEditValueServerSubprotocol;
	BOOL mCheckValueAllowSelfSigned;
	CString mEditValueLog;
	CString mEditValueSend;
	int mEditValueLogMaxLine;
};
