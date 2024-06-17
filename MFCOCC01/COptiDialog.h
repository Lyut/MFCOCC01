#pragma once
#include "afxdialogex.h"


// Finestra di dialogo COptiDialog

class COptiDialog : public CDialogEx
{
	DECLARE_DYNAMIC(COptiDialog)

public:
	COptiDialog(CWnd* pParent = nullptr);   // costruttore standard
	virtual ~COptiDialog();

// Dati della finestra di dialogo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTIDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // supporto DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
