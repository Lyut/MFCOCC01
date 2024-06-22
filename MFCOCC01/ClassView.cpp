
#include "pch.h"
#include "framework.h"
#include "MainFrm.h"
#include "ClassView.h"
#include "Resource.h"
#include "MFCOCC01.h"
#include "MFCOCC01Doc.h"


#include "MFCOCC01View.h"

class CClassViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CClassView;

	DECLARE_SERIAL(CClassViewMenuButton)

public:
	CClassViewMenuButton(HMENU hMenu = nullptr) noexcept : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CClassViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// Costruzione/Eliminazione
//////////////////////////////////////////////////////////////////////

CClassView::CClassView() noexcept
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
}

CClassView::~CClassView()
{
}

BEGIN_MESSAGE_MAP(CClassView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)
	ON_NOTIFY(NM_DBLCLK, 2, OnTreeClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Gestori di messaggi CClassView

int CClassView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Creare visualizzazioni:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	if (!m_wndClassView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("Non è stato possibile creare la Visualizzazione classi.\n");
		return -1;      // impossibile creare
	}

	// Caricare immagini:
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_wndToolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE /* È bloccato */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// Tutti i comandi verranno indirizzati tramite questo controllo anziché tramite il frame principale:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT);

	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CClassViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CClassViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != nullptr)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

	// Inserire i dati statici della visualizzazione albero (codice fittizio, niente di particolare)
	FillClassView();

	return 0;
}

afx_msg void CClassView::OnTreeClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (pNMHDR && pNMHDR->code == NM_DBLCLK && pNMHDR->hwndFrom == m_wndClassView.m_hWnd)
	{
		HTREEITEM hItem = m_wndClassView.GetSelectedItem();
		if (hItem)
		{
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int> dis(1, 508);
			CString dirCatalogue = _T(DIR_CATALOGUE);
			CString s = dirCatalogue + m_wndClassView.GetItemText(hItem);
			CMFCOCC01Doc* pDoc = GET_ACTIVE_DOC(CMFCOCC01Doc);
			if (pDoc)
				pDoc->SendOutputMessage(_T("Importazione ") + s + _T("..."));
				const aiScene* scene = pDoc->GetAssimp().ReadFile(CT2A(s.GetString()), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
				CString str;
				str.Format(_T("Importazione finita! (%i mesh)"), scene->mNumMeshes);
				pDoc->SendOutputMessage(str);
				if (!scene) {
					CString errorMsg = _T("Failed to load model: ");
					errorMsg += pDoc->GetAssimp().GetErrorString();
					AfxMessageBox(errorMsg);
					return;
				}
				pDoc->SendOutputMessage(_T("Inizio conversione oggetto..."));
				TopoDS_Shape shape = pDoc->ConvertAssimpToOpenCASCADE(scene);
				Handle(AIS_Shape) aShape = new AIS_Shape(shape);
				objList objEntry;
				objEntry.name = m_wndClassView.GetItemText(hItem);
				objEntry.shape = aShape;
				objEntry.topo_shape = shape;
				objEntry.color = static_cast<Quantity_NameOfColor>(dis(gen));
				pDoc->GetShapeList().push_back(objEntry);
				pDoc->SendOutputMessage(_T("Conversione finita!"));
				Handle(V3d_Viewer) viewer = pDoc->GetViewer();
				viewer->Invalidate();
				viewer->Redraw();
		}
	}
	*pResult = 0;
}

void CClassView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CClassView::FillClassView()
{
	fs::path directoryPath = DIR_CATALOGUE;

	if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
		AfxMessageBox(_T("Directory catalogo non esistente o comunque non valida!"));
		HTREEITEM hRoot = m_wndClassView.InsertItem(_T("Catalogo (offline)"), 0, 0);
	}
	else {
		HTREEITEM hRoot = m_wndClassView.InsertItem(_T("Catalogo"), 0, 0);
		m_wndClassView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
		// TODO: recurse
		for (const auto& entry : fs::directory_iterator(directoryPath)) {
			if (fs::is_directory(entry.path())) {
				HTREEITEM hDir = m_wndClassView.InsertItem(entry.path().filename().c_str(), 1, 1, hRoot);
				for (const auto& subentry : fs::directory_iterator(entry.path())) {
					if (fs::is_regular_file(subentry.path())) {
						m_wndClassView.InsertItem(subentry.path().filename().c_str(), 3, 3, hDir);
					}
				}
			}
			else if (fs::is_regular_file(entry.path())) {
				m_wndClassView.InsertItem(entry.path().filename().c_str(), 3, 3, hRoot);
			}
		}
		m_wndClassView.Expand(hRoot, TVE_EXPAND);
	}
}

void CClassView::InsertItem(LPCTSTR str) {
	HTREEITEM hRoot = m_wndClassView.InsertItem(str, 0, 0);
	m_wndClassView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
}

void CClassView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndClassView;
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
	CMenu menu;
	menu.LoadMenu(IDR_POPUP_SORT);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}
}

void CClassView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndClassView.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CClassView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CClassView::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	CClassViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != nullptr)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}
}

void CClassView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CClassView::OnClassAddMemberFunction()
{
	AfxMessageBox(_T("Aggiungi funzione membro..."));
}

void CClassView::OnClassAddMemberVariable()
{
	// TODO: aggiungere qui il codice del gestore di comandi
}

void CClassView::OnClassDefinition()
{
	// TODO: aggiungere qui il codice del gestore di comandi
}

void CClassView::OnClassProperties()
{
	// TODO: aggiungere qui il codice del gestore di comandi
}

void CClassView::OnNewFolder()
{
	AfxMessageBox(_T("Nuova cartella..."));
}

void CClassView::OnPaint()
{
	CPaintDC dc(this); // contesto di dispositivo per il disegno

	CRect rectTree;
	m_wndClassView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CClassView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndClassView.SetFocus();
}

void CClassView::OnChangeVisualStyle()
{
	m_ClassViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

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

	m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ClassViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndClassView.SetImageList(&m_ClassViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* Bloccato */);
}
