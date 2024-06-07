// COptiDiag.cpp: file di implementazione
//

#include "pch.h"
#include "MFCOCC01.h"
#include "COptiDiag.h"
#include "afxdialogex.h"


// Finestra di dialogo COptiDiag

IMPLEMENT_DYNAMIC(COptiDiag, CDialogEx)

COptiDiag::COptiDiag(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OPTIDIAG, pParent)
{

}

COptiDiag::~COptiDiag()
{
}

void COptiDiag::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptiDiag, CDialogEx)
	ON_BN_CLICKED(IDOK, &COptiDiag::OnBnClickedOk)
END_MESSAGE_MAP()


// Gestori di messaggi di COptiDiag


void COptiDiag::OnBnClickedOk()
{
	// TODO: aggiungere qui il codice del gestore di notifiche del controllo
	CDialogEx::OnOK();
}
