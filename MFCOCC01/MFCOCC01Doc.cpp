
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
	m_hViewer->SetDefaultBgGradientColors(Quantity_NOC_ALICEBLUE, Quantity_NOC_CORNSILK1, Aspect_GradientFillMethod_Vertical);
	//改变背景颜色

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

void CMFCOCC01Doc::StartSimulation()
{
	std::vector<std::pair<Standard_Real, Standard_Real>> panel_dimensions = {
		{20.0, 50.0}, {20.0, 50.0}, {5.0, 5.0}, {5.0, 5.0}, {5.0, 5.0}, {15.0, 50.0}, {5.0, 5.0}
	};
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(1, 508);

	for (const auto& dimensions : panel_dimensions) {
		Standard_Real width = dimensions.first;
		Standard_Real height = dimensions.second;
		gp_Pnt start_point = findEmptyPosition(width, height);
		int randomIndex = dis(gen);
		TopoDS_Shape smaller_panel = BRepPrimAPI_MakeBox(start_point, gp_Pnt(start_point.X() + width, start_point.Y() + height, start_point.Z() + 10.0)).Shape();
		if (!panelOverlaps(smaller_panel)) {
				Panel newPanel;
				newPanel.origin = start_point;
				newPanel.height = dimensions.first;
				newPanel.width = dimensions.second;
				newPanel.thickness = 10;
				newPanel.color = static_cast<Quantity_NameOfColor>(randomIndex);

				panelList.push_back(newPanel);
		}
	}
}

gp_Pnt CMFCOCC01Doc::findEmptyPosition(Standard_Real width, Standard_Real height)
{
	Standard_Real step = 1.0;
	for (Standard_Real x = 0.0; x <= 300 - width; x += step) {
		for (Standard_Real y = 0.0; y <= 100 - height; y += step) {
			gp_Pnt start_point(x, y, 0.0);
			if (!panelOverlaps(BRepPrimAPI_MakeBox(start_point, gp_Pnt(start_point.X() + width, start_point.Y() + height, start_point.Z() + 10.0)).Shape())) {
				return start_point;
			}
			// Try rotating it
			if (!panelOverlaps(BRepPrimAPI_MakeBox(start_point, gp_Pnt(start_point.X() + height, start_point.Y() + width, start_point.Z() + 10.0)).Shape())) {
				return start_point;
			}
		}
	}
	return gp_Pnt(); // Return null point if no empty position found
}

bool CMFCOCC01Doc::panelOverlaps(TopoDS_Shape panel)
{
	Bnd_Box panel_bbox, existing_panel_bbox;
	BRepBndLib::Add(panel, panel_bbox);
	for (const auto& existing_panel : panelList) {
		BRepPrimAPI_MakeBox mkBox(existing_panel.origin, existing_panel.height, existing_panel.width, existing_panel.thickness);
		TopoDS_Shape Box = mkBox.Shape();
		BRepBndLib::Add(Box, existing_panel_bbox);
		if (panel_bbox.IsOut(existing_panel_bbox) == 0 || existing_panel_bbox.IsOut(panel_bbox) == 0) {
			return true;
		}
	}
	return false;
}


BOOL CMFCOCC01Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: aggiungere qui il codice di reinizializzazione
	// (nei documenti SDI verrà riutilizzato questo documento).
	InitOCC();
	std::thread simulationThread(&CMFCOCC01Doc::StartSimulation, this);
	simulationThread.detach();

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