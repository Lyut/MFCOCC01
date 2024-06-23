
#include "pch.h"
#include "framework.h"
#include "mainfrm.h"
#include "FileView.h"
#include "Resource.h"
#include "MFCOCC01.h"

#include "MFCOCC01Doc.h"
#include "MFCOCC01View.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView

CFileView::CFileView() noexcept
{
	CDatabase db;
	CString strConnect = _T("Driver={Oracle in OraDB21Home2};Dbq=localhost:1521/ORCLPDB;UID=apiuser;PWD=porcodio99");
	TRY
	{
		if (db.OpenEx(strConnect))
		{
			// Connection succeeded
			// Perform your database operations here
		}
	}
		CATCH(CException, e)
	{
		e->ReportError();
		// Not necessary to delete the exception e.
	}
	END_CATCH

}

CFileView::~CFileView()
{
}

BEGIN_MESSAGE_MAP(CFileView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_NOTIFY(NM_DBLCLK, 4, OnTreeClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Gestori di messaggi CWorkspaceBar

int CFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Creare visualizzazione:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("Non è stato possibile creare la visualizzazione file.\n");
		return -1;      // impossibile creare
	}

	// Caricare immagini visualizzazione:
	m_FileViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* È bloccato */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// Tutti i comandi verranno indirizzati tramite questo controllo anziché tramite il frame principale:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// Inserire i dati statici della visualizzazione albero (codice fittizio, niente di particolare)
	FillFileView();
	AdjustLayout();

	return 0;
}

void CFileView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CFileView::FillFileView()
{
	fs::path directoryPath = DIR_CATALOGUE;

	if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
		AfxMessageBox(_T("Directory catalogo non esistente o comunque non valida!"));
		HTREEITEM hRoot = m_wndFileView.InsertItem(_T("Catalogo (offline)"), 0, 0);
	}
	else {
		HTREEITEM hRoot = m_wndFileView.InsertItem(_T("Catalogo"), 0, 0);
		m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
		// TODO: recurse
		for (const auto& entry : fs::directory_iterator(directoryPath)) {
			if (fs::is_directory(entry.path())) {
				HTREEITEM hDir = m_wndFileView.InsertItem(entry.path().filename().c_str(), 0, 0, hRoot);
				for (const auto& subentry : fs::directory_iterator(entry.path())) {
					if (fs::is_regular_file(subentry.path())) {
						m_wndFileView.InsertItem(subentry.path().filename().c_str(), 2, 2, hDir);
					}
				}
			}
			else if (fs::is_regular_file(entry.path())) {
				m_wndFileView.InsertItem(entry.path().filename().c_str(), 2, 2, hRoot);
			}
		}
		m_wndFileView.Expand(hRoot, TVE_EXPAND);
	}

	/*HTREEITEM hRoot = m_wndFileView.InsertItem(_T("File FakeApp"), 0, 0);
	m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	HTREEITEM hSrc = m_wndFileView.InsertItem(_T("File di origine FakeApp"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("FakeApp.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("FakeApp.rc"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("FakeAppDoc.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("FakeAppView.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("MainFrm.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("pch.cpp"), 1, 1, hSrc);

	HTREEITEM hInc = m_wndFileView.InsertItem(_T("File di intestazione FakeApp"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("FakeApp.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("FakeAppDoc.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("FakeAppView.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("Resource.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("MainFrm.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("pch.h"), 2, 2, hInc);

	HTREEITEM hRes = m_wndFileView.InsertItem(_T("File di risorse FakeApp"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("FakeApp.ico"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("FakeApp.rc2"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("FakeAppDoc.ico"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("FakeToolbar.bmp"), 2, 2, hRes);

	m_wndFileView.Expand(hRoot, TVE_EXPAND);
	m_wndFileView.Expand(hSrc, TVE_EXPAND);
	m_wndFileView.Expand(hInc, TVE_EXPAND);*/
}


afx_msg void CFileView::OnTreeClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (pNMHDR && pNMHDR->code == NM_DBLCLK && pNMHDR->hwndFrom == m_wndFileView.m_hWnd)
		OnFileOpen();
	*pResult = 0;
}

void CFileView::InsertItem(LPCTSTR str) {
	HTREEITEM hRoot = m_wndFileView.InsertItem(str, 0, 0);
	m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
}

void CFileView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndFileView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Scegliere la voce selezionata:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != nullptr)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CFileView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFileView.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CFileView::OnProperties()
{
	AfxMessageBox(_T("Proprietà..."));

}

void CFileView::OnFileOpen()
{
	HTREEITEM hItem = m_wndFileView.GetSelectedItem();
	if (hItem)
	{
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dis(1, 508);
		CString dirCatalogue = _T(DIR_CATALOGUE);
		CString s = dirCatalogue + m_wndFileView.GetItemText(hItem);
		CMFCOCC01Doc* pDoc = GET_ACTIVE_DOC(CMFCOCC01Doc);
		if (pDoc)
			pDoc->SendOutputMessage(_T(__FILE__ " >> Importazione ") + s + _T("..."));
		const aiScene* scene = pDoc->GetAssimp().ReadFile(CT2A(s.GetString()), aiProcess_GenSmoothNormals);
		CString str;
		str.Format(_T(__FILE__ " >> Importazione finita! (%i mesh)"), scene->mNumMeshes);
		pDoc->SendOutputMessage(str);
		if (!scene) {
			CString errorMsg = _T("Failed to load model: ");
			errorMsg += pDoc->GetAssimp().GetErrorString();
			AfxMessageBox(errorMsg);
			return;
		}
		pDoc->SendOutputMessage(_T(__FILE__ " >> Inizio conversione oggetto..."));
		TopoDS_Shape shape = pDoc->ConvertAssimpToOpenCASCADE(scene);
		Handle(AIS_Shape) aShape = new AIS_Shape(shape);
		objList objEntry;
		objEntry.name = m_wndFileView.GetItemText(hItem);
		objEntry.shape = aShape;
		objEntry.topo_shape = shape;
		objEntry.color = static_cast<Quantity_NameOfColor>(dis(gen));
		objEntry.originalColor = objEntry.color;
		objEntry.textLabel = new AIS_TextLabel();
		pDoc->GetShapeList().push_back(objEntry);
		pDoc->SendOutputMessage(_T(__FILE__ " >> Conversione finita!"));
		CMainFrame* pFrame = pDoc->GetMainFrame();
		if (pFrame != nullptr)
		{
			CMFCOCC01View* pMFCView = nullptr;
			POSITION pos = pDoc->GetFirstViewPosition();
			while (pos != nullptr)
			{
				CView* pView = pDoc->GetNextView(pos);
				if (pView->IsKindOf(RUNTIME_CLASS(CMFCOCC01View)))
				{
					pMFCView = (CMFCOCC01View*)pView;
				}
			}
			if (pMFCView != nullptr)
			{
				pMFCView->SendMessage(WM_REDRAW_VIEW);
				pMFCView->SendMessage(WM_FITALL);
				CString str;
				str.Format(_T("%s 0x%p"), objEntry.name, objEntry.shape);
				pFrame->GetClassView().InsertItem(str);
			}
		}
	}
}

void CFileView::OnFileOpenWith()
{
	// TODO: aggiungere qui il codice del gestore di comandi
}

void CFileView::OnDummyCompile()
{
	// TODO: aggiungere qui il codice del gestore di comandi
}

void CFileView::OnEditCut()
{
	// TODO: aggiungere qui il codice del gestore di comandi
}

void CFileView::OnEditCopy()
{
	// TODO: aggiungere qui il codice del gestore di comandi
}

void CFileView::OnEditClear()
{
	// TODO: aggiungere qui il codice del gestore di comandi
}

void CFileView::OnPaint()
{
	CPaintDC dc(this); // contesto di dispositivo per il disegno

	CRect rectTree;
	m_wndFileView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CFileView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndFileView.SetFocus();
}

void CFileView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* Bloccato */);

	m_FileViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Impossibile caricare bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_FileViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
}


