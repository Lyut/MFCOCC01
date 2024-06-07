
// MFCOCC01.cpp: definisce i comportamenti delle classi per l'applicazione.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "MFCOCC01.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "MFCOCC01Doc.h"
#include "MFCOCC01View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCOCC01App

BEGIN_MESSAGE_MAP(CMFCOCC01App, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CMFCOCC01App::OnAppAbout)
	// Comandi di documenti basati su file standard
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Comando di stampa standard
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// Costruzione di CMFCOCC01App

CMFCOCC01App::CMFCOCC01App() noexcept
{
	m_bHiColorIcons = TRUE;


	m_nAppLook = 0;
	// supporto Gestione riavvio
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// Se l'applicazione è compilata mediante il supporto CLR (/clr):
	//     1) Questa ipostazione è necessaria per il corretto funzionamento del supporto di Gestione riavvio.
	//     2) Nel progetto è necessario aggiungere un riferimento a System.Windows.Forms per eseguire la compilazione.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: sostituire la stringa dell'ID applicazione seguente con una stringa di ID univoca; il formato consigliato
	// per la stringa è NomeSocietà.NomeProdotto.Sottoprodotto.Versione
	SetAppID(_T("MFCOCC01.AppID.NoVersion"));

	// TODO: aggiungere qui il codice di costruzione.
	// Inserire l'inizializzazione significativa in InitInstance.
}

// Unico oggetto CMFCOCC01App

CMFCOCC01App theApp;


// Inizializzazione di CMFCOCC01App


BOOL CMFCOCC01App::InitInstance()
{
	// InitCommonControlsEx() è necessario in Windows XP se nel manifesto
	// di un'applicazione è specificato l'utilizzo di ComCtl32.dll versione 6 o successiva per abilitare
	// gli stili visuali.  In caso contrario, non sarà possibile creare finestre.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Effettuare questa impostazione in modo da includere tutte le classi di controlli comuni da utilizzare
	// nell'applicazione.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	try
	{
		Handle(Aspect_DisplayConnection) aDisplayConnection;
		m_hGraphicDriver = new OpenGl_GraphicDriver(aDisplayConnection);

		if (m_hGraphicDriver.IsNull()) {
			// Handle the failure, for example, display an error message
			AfxMessageBox(_T("Failed to initialize graphic driver"), MB_ICONSTOP);
			// Optionally, exit the application or take appropriate action
			// ExitProcess(1);
		}
	}
	catch (Standard_Failure)
	{
		AfxMessageBox(_T("Fatal error during graphic initialization"), MB_ICONSTOP);
		ExitProcess(1);
	}


	CWinAppEx::InitInstance();


	// Inizializzare le librerie OLE.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// Per usare il controllo RichEdit, è richiesto AfxInitRichEdit2()
	// AfxInitRichEdit2();

	// Inizializzazione standard
	// Se non si utilizzano queste funzionalità e si desidera ridurre la dimensione
	// dell'eseguibile finale, è necessario rimuovere dal seguente codice
	// le specifiche routine di inizializzazione che non sono necessarie.
	// Modificare la chiave del Registro di sistema in cui sono memorizzate le impostazioni
	// TODO: è necessario modificare questa stringa in modo appropriato,
	// inserendo ad esempio il nome della società o dell'organizzazione.
	SetRegistryKey(_T("Applicazioni locali generate tramite la Creazione guidata applicazioni"));
	LoadStdProfileSettings(4);  // Caricare le opzioni del file INI standard (inclusa MRU).


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Registrare i modelli di documenti dell'applicazione.  I modelli di documenti
	//  funzionano da connessione tra documenti, finestre cornice e viste.
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_MFCOCC01TYPE,
		RUNTIME_CLASS(CMFCOCC01Doc),
		RUNTIME_CLASS(CChildFrame), // frame MDI figlio personalizzato
		RUNTIME_CLASS(CMFCOCC01View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// creare finestra cornice MDI principale.
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// Analizzare la riga di comando per i comandi shell standard, DDE, apri file
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// Invia i comandi specificati nella riga di comando.  Restituisce FALSE se
	// l'applicazione è stata avviata con l'opzione /RegServer, /Register, /Unregserver o /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// La finestra principale è stata inizializzata, quindi è possibile visualizzarla e aggiornarla.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

Handle_Graphic3d_GraphicDriver CMFCOCC01App::GetGraphicDriver() const
{
	return m_hGraphicDriver;
}

int CMFCOCC01App::ExitInstance()
{
	//TODO: gestire le altre risorse eventualmente aggiunte.
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// Gestori di messaggi di CMFCOCC01App


// finestra di dialogo CAboutDlg utilizzata per visualizzare le informazioni sull'applicazione.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Dati della finestra di dialogo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // supporto DDX/DDV

// Implementazione
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// comando dell'applicazione per eseguire la finestra di dialogo
void CMFCOCC01App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// Metodi di caricamento/salvataggio personalizzazione di CMFCOCC01App

void CMFCOCC01App::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CMFCOCC01App::LoadCustomState()
{
}

void CMFCOCC01App::SaveCustomState()
{
}

CDocument* CMFCOCC01App::GenericGetActiveDocument(CRuntimeClass* pClass)
{
    ASSERT(NULL != pClass); // must be not NULL and derived from CDocument.
    ASSERT(pClass->IsDerivedFrom(RUNTIME_CLASS(CDocument))); 

    CDocument* pDoc = NULL;
    CWnd* pWndMain = AfxGetMainWnd();
    if(NULL != pWndMain)
    {
        if(pWndMain->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
        {
            // MDI application, so first we have to get the active MDI child frame.
            CFrameWnd* pFrame = ((CMDIFrameWnd*)pWndMain)->MDIGetActive();
            if(NULL != pFrame)
            {
                CDocument* pActiveDoc = pFrame->GetActiveDocument();
                if((NULL != pActiveDoc) && pActiveDoc->IsKindOf(pClass))
                {
                    // The found document is of required type
                    pDoc = pActiveDoc;
                }
            }
        }
        else if(pWndMain->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
        {
            // SDI appllication so main window is the active frame. 
            pDoc = ((CFrameWnd*)pWndMain)->GetActiveDocument();
        }
        else
        {
            ASSERT(FALSE); // Neither MDI nor SDI application.
        }
    }
    return pDoc;
}

// Gestori di messaggi di CMFCOCC01App


