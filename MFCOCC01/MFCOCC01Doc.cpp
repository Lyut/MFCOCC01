
// MFCOCC01Doc.cpp: implementazione della classe CMFCOCC01Doc
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS può essere definito in un'anteprima di implementazione del progetto ATL, nei gestori di anteprime
// e della ricerca e consente di condividere il codice dei documenti con il progetto in questione.
#ifndef SHARED_HANDLERS
#include "MFCOCC01.h"
#endif

#include "MFCOCC01Doc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCOCC01Doc

IMPLEMENT_DYNCREATE(CMFCOCC01Doc, CDocument)

BEGIN_MESSAGE_MAP(CMFCOCC01Doc, CDocument)
END_MESSAGE_MAP()


// Costruzione/distruzione di CMFCOCC01Doc

CMFCOCC01Doc::CMFCOCC01Doc() noexcept
{
	// TODO: aggiungere qui il codice di costruzione unico
    num_panels_to_generate = 10; // Default value, adjust as needed
    binaryTreeRoot = nullptr; // Initialize the binary tree root to null

}

CMFCOCC01Doc::~CMFCOCC01Doc()
{
}

BOOL CMFCOCC01Doc::InitOCC()
{
	Handle(Graphic3d_GraphicDriver) aGraphicDriver = ((CMFCOCC01App*)AfxGetApp())->GetGraphicDriver();
	m_hViewer = new V3d_Viewer(aGraphicDriver);

	m_hViewer->SetDefaultLights();
	m_hViewer->SetLightOn();
	m_hViewer->SetDefaultBgGradientColors(Quantity_NOC_ALICEBLUE, Quantity_NOC_GRAY89, Aspect_GradientFillMethod_Vertical);
	//改变背景颜色
	//m_hViewer->SetDefaultTypeOfView(V3d_ORTHOGRAPHIC);
	m_hAISContext = new AIS_InteractiveContext(m_hViewer);  //创建一个交互文档
	m_hAISContext->DefaultDrawer()->SetWireDraw(Standard_True);
	m_hAISContext->DefaultDrawer()->SetFaceBoundaryDraw(Standard_True);
	//m_hAISContext->DefaultDrawer()->
	//m_hAISContext->DefaultDrawer()->SetUIsoAspect(11);

  //这里设置实体的显示模式
	m_hAISContext->SetDisplayMode(AIS_Shaded, Standard_True);
	m_hAISContext->SetAutomaticHilight(Standard_True);
	return TRUE;
}

bool compareByArea(const std::pair<Standard_Real, Standard_Real>& a, const std::pair<Standard_Real, Standard_Real>& b) {
	return (a.first * a.second) > (b.first * b.second);
}

void CMFCOCC01Doc::insertPanel(BinaryTreeNode*& root, Panel* panel) {
    // If the tree is empty, create a new node
    if (root == nullptr) {
        root = new BinaryTreeNode;
        root->panel = panel;
        root->left = nullptr;
        root->right = nullptr;
    }
    else {
        // If the current node is not a leaf node, try inserting into its children
        if (root->left != nullptr && root->right != nullptr) {
            // Try inserting into the left subtree first
            insertPanel(root->left, panel);
            // If insertion into the left subtree fails, try the right subtree
            if (root->left->panel == nullptr) {
                insertPanel(root->right, panel);
            }
        }
        else {
            // If the current node is a leaf node and it's unoccupied, insert the panel here
            if (root->panel == nullptr) {
                root->panel = panel;
            }
            else {
                // The current leaf node is occupied, so split it into two children
                root->left = new BinaryTreeNode;
                root->left->panel = nullptr;
                root->left->left = nullptr;
                root->left->right = nullptr;

                root->right = new BinaryTreeNode;
                root->right->panel = nullptr;
                root->right->left = nullptr;
                root->right->right = nullptr;

                // Now recursively try inserting the panel into the new children
                insertPanel(root->left, panel);
            }
        }
    }
}

void CMFCOCC01Doc::buildBinaryTree(std::vector<Panel>& panelList, BinaryTreeNode*& root) {
    // Iterate through the list of panels and insert each panel into the binary tree
    for (Panel& panel : panelList) {
        insertPanel(root, &panel);
    }
}

// Function to recursively deallocate memory for the binary tree nodes
void destroyBinaryTree(BinaryTreeNode*& root) {
    if (root == nullptr) {
        return;
    }

    // Recursively destroy the left subtree
    destroyBinaryTree(root->left);
    // Recursively destroy the right subtree
    destroyBinaryTree(root->right);

    // Delete the current node
    delete root;
    // Set the pointer to null to avoid dangling references
    root = nullptr;
}

void CMFCOCC01Doc::StartSimulation() {
    CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());

    // Clear any existing binary tree
    destroyBinaryTree(binaryTreeRoot);

    // Clear any existing panel list and free spaces
    panelList.clear();
    freeSpaces.clear();

    // Initialize free space with the entire area
    freeSpaces.insert({ 0.0, 0.0, 300.0, 100.0 });

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1, 508);
    Standard_Real minWidth = 10.0; // Minimum width of a panel
    Standard_Real maxWidth = 50.0; // Maximum width of a panel
    Standard_Real minHeight = 10.0; // Minimum height of a panel
    Standard_Real maxHeight = 100.0; // Maximum height of a panel

    // Generate random values within the specified range
   // Standard_Real width = minWidth + (maxWidth - minWidth) * (static_cast<Standard_Real>(rand()) / RAND_MAX);
   // Standard_Real height = minHeight + (maxHeight - minHeight) * (static_cast<Standard_Real>(rand()) / RAND_MAX);

    for (int i = 0; i < num_panels_to_generate; ++i) {
        Standard_Real width = minWidth + (maxWidth - minWidth) * (static_cast<Standard_Real>(rand()) / RAND_MAX);
        Standard_Real height = minHeight + (maxHeight - minHeight) * (static_cast<Standard_Real>(rand()) / RAND_MAX);
        bool found;
        gp_Pnt start_point = findEmptyPosition(width, height, found);

        if (found) {
            Panel* newPanel = new Panel;
            newPanel->origin = start_point;
            newPanel->height = height;
            newPanel->width = width;
            newPanel->thickness = 10;
            newPanel->color = static_cast<Quantity_NameOfColor>(dis(gen));

            // Insert the new panel into the binary tree
            insertPanel(binaryTreeRoot, newPanel);

            // Update free spaces
            updateFreeSpaces(start_point, width, height);

            // Add the panel to the panel list
            panelList.push_back(*newPanel);

            // Notify the UI of the inserted panel
            CString panelStr;
            panelStr.Format(_T("Pannello (%f, %f)"), (double&)width, (double&)height);
            InsertItemMsg* pData = new InsertItemMsg;
            pData->strItem = panelStr;
            if (pMainFrame)
                pMainFrame->SendMessage(WM_INSERTITEM_MESSAGE, 0, (LPARAM)pData);
            delete pData;
        }
    }
}

gp_Pnt CMFCOCC01Doc::findEmptyPosition(Standard_Real width, Standard_Real height, bool& found) {
    for (const auto& space : freeSpaces) {
        if (space.width >= width && space.height >= height) {
            found = true;
            return gp_Pnt(space.x, space.y, 0.0);
        }
        if (space.width >= height && space.height >= width) { // Try rotating
            found = true;
            return gp_Pnt(space.x, space.y, 0.0);
        }
    }
    found = false;
    return gp_Pnt(); // Return null point if no empty position found
}

bool CMFCOCC01Doc::panelOverlaps(const TopoDS_Shape& panel) {
    Bnd_Box panel_bbox;
    BRepBndLib::Add(panel, panel_bbox);

    for (const auto& existing_panel : panelList) {
        TopoDS_Shape existing_box = BRepPrimAPI_MakeBox(existing_panel.origin, existing_panel.width, existing_panel.height, existing_panel.thickness).Shape();
        Bnd_Box existing_bbox;
        BRepBndLib::Add(existing_box, existing_bbox);

        if (!panel_bbox.IsOut(existing_bbox) || !existing_bbox.IsOut(panel_bbox)) {
            return true;
        }
    }
    return false;
}

void CMFCOCC01Doc::updateFreeSpaces(const gp_Pnt& point, Standard_Real width, Standard_Real height) {
    std::set<FreeSpace> newFreeSpaces;

    for (const auto& space : freeSpaces) {
        if (point.X() == space.x && point.Y() == space.y) {
            // Split the remaining free space
            if (space.width > width) {
                newFreeSpaces.insert({ space.x + width, space.y, space.width - width, height });
            }
            if (space.height > height) {
                newFreeSpaces.insert({ space.x, space.y + height, width, space.height - height });
            }
            if (space.width > width && space.height > height) {
                newFreeSpaces.insert({ space.x + width, space.y + height, space.width - width, space.height - height });
            }
        }
        else {
            newFreeSpaces.insert(space);
        }
    }

    freeSpaces = newFreeSpaces;
}

BOOL CMFCOCC01Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: aggiungere qui il codice di reinizializzazione
	// (nei documenti SDI verrà riutilizzato questo documento).
	if (InitOCC()) {
		std::thread simulationThread(&CMFCOCC01Doc::StartSimulation, this);
		simulationThread.detach();
	}

	return TRUE;
}




// Serializzazione di CMFCOCC01Doc

void CMFCOCC01Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: aggiungere qui il codice di archiviazione.
	}
	else
	{
		// TODO: aggiungere qui il codice di caricamento.
	}
}

#ifdef SHARED_HANDLERS

// Supporto per anteprime
void CMFCOCC01Doc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modificare il codice per creare i dati del documento
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Supporto per gestori di ricerca
void CMFCOCC01Doc::InitializeSearchContent()
{
	CString strSearchContent;
	// Consente di impostare i contenuti di ricerca dai dati del documento.
	// Le parti di contenuto devono essere separata da ";"

	// Ad esempio:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CMFCOCC01Doc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// Diagnostica di CMFCOCC01Doc

#ifdef _DEBUG
void CMFCOCC01Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMFCOCC01Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Comandi di CMFCOCC01Doc
