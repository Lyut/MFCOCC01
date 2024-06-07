#pragma once


// Finestra di dialogo COptiDiag

class COptiDiag : public CDialogEx
{
	DECLARE_DYNAMIC(COptiDiag)

public:
	COptiDiag(CWnd* pParent = nullptr);   // costruttore standard
	virtual ~COptiDiag();

// Dati della finestra di dialogo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTIDIAG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // supporto DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
