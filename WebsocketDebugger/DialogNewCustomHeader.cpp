#include "pch.h"
#include "WebsocketDebugger.h"
#include "afxdialogex.h"
#include "DialogNewCustomHeader.h"

IMPLEMENT_DYNAMIC(CDialogNewCustomHeader, CDialogEx)

CDialogNewCustomHeader::CDialogNewCustomHeader (CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CUSTOMHEADERS_ADD, pParent)
	, mEditValueCHKey (_T (""))
	, mEditValueCHValue (_T (""))
{

}

CDialogNewCustomHeader::~CDialogNewCustomHeader ()
{
}

void CDialogNewCustomHeader::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange (pDX);
	DDX_Text (pDX, IDC_EDIT_CH_KEY, mEditValueCHKey);
	DDX_Text (pDX, IDC_EDIT_CH_VALUE, mEditValueCHValue);
	DDX_Control (pDX, IDC_EDIT_CH_KEY, mEditControlCHKey);
	DDX_Control (pDX, IDC_EDIT_CH_VALUE, mEditControlCHValue);
}


BEGIN_MESSAGE_MAP(CDialogNewCustomHeader, CDialogEx)
	ON_BN_CLICKED (IDOK, &CDialogNewCustomHeader::OnBnClickedOk)
END_MESSAGE_MAP()


void CDialogNewCustomHeader::OnBnClickedOk ()
{
	UpdateData (true);

	mEditValueCHKey.Trim ();
	mEditValueCHValue.Trim ();

	if (mEditValueCHKey.GetLength () <= 0) {
		mEditControlCHKey.SetFocus ();
		UpdateData (false);
		return;
	}

	if (mEditValueCHValue.GetLength () <= 0) {
		mEditControlCHValue.SetFocus ();
		UpdateData (false);
		return;
	}

	CDialogEx::OnOK ();
}

void CDialogNewCustomHeader::SetModifyData (CString key, CString value)
{
	mEditValueCHKey = key;
	mEditValueCHValue = value;
}
