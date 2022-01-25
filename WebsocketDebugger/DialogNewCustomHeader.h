#pragma once
#include "afxdialogex.h"

class CDialogNewCustomHeader : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogNewCustomHeader)

public:
	CDialogNewCustomHeader (CWnd* pParent = nullptr);
	virtual ~CDialogNewCustomHeader ();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CUSTOMHEADERS_ADD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk ();
	CString mEditValueCHKey;
	CString mEditValueCHValue;
	CEdit mEditControlCHKey;
	CEdit mEditControlCHValue;
};
