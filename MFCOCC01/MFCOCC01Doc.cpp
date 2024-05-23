
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

void CMFCOCC01Doc::StartSimulation() {
    CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1, 508);

    std::vector<Node> blocks = {
        Node("Pannello1", 5.0, 5.0),
        Node("Pannello2", 10.0, 10.0),
		Node("Pannello3", 100.0, 155.0)
    };

    packer.fit(blocks);
	int thickness = 2;
    for (const auto& block : blocks) {
		SendInsertItem(_T("%S (%f, %f)"), block.name.c_str(), block.fit->x, block.fit->y);
		SendOutputMessage(_T("Creato pannello %S (x: %f, y: %f) (W: %f H: %f)"), block.name.c_str(), block.fit->x, block.fit->y, block.fit->w, block.fit->h);

        Panel* newPanel = new Panel;
        newPanel->origin = gp_Pnt(block.fit->x, block.fit->y, 0);
        newPanel->height = block.fit->h;
        newPanel->width = block.fit->w;
        newPanel->thickness = thickness + 5;
        newPanel->color = static_cast<Quantity_NameOfColor>(dis(gen));

        // Add the panel to the panel list
        panelList.push_back(*newPanel);
    }

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

void CMFCOCC01Doc::SendOutputMessage(LPCTSTR str, ...) {
	pMainFrame = GetMainFrame();
	va_list args;
	va_start(args, str);
	CString msgStr;
	msgStr.FormatV(str, args);
	va_end(args);
	OutputMessageMsg* pDataMsg = new OutputMessageMsg;
	pDataMsg->message = msgStr;
	if (pMainFrame != nullptr) {
		pMainFrame->SendMessage(WM_OUTPUTMSG_MESSAGE, 0, (LPARAM)pDataMsg);
	}
	delete pDataMsg;
}

void CMFCOCC01Doc::SendInsertItem(LPCTSTR str, ...) {
	va_list args;
	va_start(args, str);
	CString panelStr;
	panelStr.FormatV(str, args);
	va_end(args);
	InsertItemMsg* pData = new InsertItemMsg;
	pData->strItem = panelStr;
	if (pMainFrame) {
		pMainFrame->SendMessage(WM_INSERTITEM_MESSAGE, 0, (LPARAM)pData);
	}
	delete pData;
}