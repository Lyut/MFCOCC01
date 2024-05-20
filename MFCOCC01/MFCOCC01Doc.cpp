
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


void CMFCOCC01Doc::StartSimulation() {
    CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());

    panel_dimensions = {
        {20.0, 50.0}, {20.0, 50.0}, {5.0, 5.0}, {5.0, 5.0}, {5.0, 5.0}, {15.0, 50.0}, {5.0, 5.0}
    };

    std::sort(panel_dimensions.begin(), panel_dimensions.end(), compareByArea);

    // Initialize free space with the entire area
    freeSpaces.insert({ 0.0, 0.0, 300.0, 100.0 });

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1, 508);

    for (const auto& dimensions : panel_dimensions) {
        Standard_Real width = dimensions.first;
        Standard_Real height = dimensions.second;
        bool found;
        gp_Pnt start_point = findEmptyPosition(width, height, found);

        CString message;
        message.Format(_T("Cerco di allocare pannello di dimensioni: %f, %f"), (double&)width, (double&)height);
        OutputMessageMsg* pData = new OutputMessageMsg;
        pData->message = message;
        if (pMainFrame)
            pMainFrame->SendMessage(WM_OUTPUTMSG_MESSAGE, 0, (LPARAM)pData);

        if (found) {
            int randomIndex = dis(gen);
            TopoDS_Shape smaller_panel = BRepPrimAPI_MakeBox(start_point, gp_Pnt(start_point.X() + width, start_point.Y() + height, start_point.Z() + 10.0)).Shape();
            if (!panelOverlaps(smaller_panel)) {
                Panel newPanel;
                newPanel.origin = start_point;
                newPanel.height = height;
                newPanel.width = width;
                newPanel.thickness = 10;
                newPanel.color = static_cast<Quantity_NameOfColor>(randomIndex);

                panelList.push_back(newPanel);
                CString panelStr;
                panelStr.Format(_T("Pannello (%f, %f)"), (double&)width, (double&)height);
                InsertItemMsg* pData = new InsertItemMsg;
                pData->strItem = panelStr;
                if (pMainFrame)
                    pMainFrame->SendMessage(WM_INSERTITEM_MESSAGE, 0, (LPARAM)pData);
                

                // Update free spaces
                updateFreeSpaces(start_point, width, height);
            }
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
