// COptiDialog.cpp: file di implementazione
//

#include "pch.h"
#include "MFCOCC01.h"
#include "afxdialogex.h"
#include "COptiDialog.h"
#include "MainFrm.h"
#include "MFCOCC01Doc.h"


// Finestra di dialogo COptiDialog

IMPLEMENT_DYNAMIC(COptiDialog, CDialogEx)

COptiDialog::COptiDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OPTIDIALOG, pParent)
{

}

COptiDialog::~COptiDialog()
{
}

void COptiDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptiDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &COptiDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// Gestori di messaggi di COptiDialog


void COptiDialog::OnBnClickedOk()
{
	// TODO: aggiungere qui il codice del gestore di notifiche del controllo
    // Retrieve the document using a safer method
    CMFCOCC01Doc* pDoc = CMFCOCC01Doc::GetDocument();

    // Check if the document pointer is valid
    if (pDoc != nullptr) {
        pDoc->StartSimulation();
    }
    else {
        AfxMessageBox(_T("Failed to retrieve the document."));
    }

	CDialogEx::OnOK();
}
