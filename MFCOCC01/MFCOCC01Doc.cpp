
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

#include "MFCOCC01View.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCOCC01Doc

IMPLEMENT_DYNCREATE(CMFCOCC01Doc, CDocument)

BEGIN_MESSAGE_MAP(CMFCOCC01Doc, CDocument)
	ON_COMMAND(ID_OTTIMIZZAZIONE_INIZIASIMULAZIONE, &CMFCOCC01Doc::OnOttimizzazioneIniziasimulazione)
	ON_COMMAND(ID_MODIFICA_MUOVI, &CMFCOCC01Doc::OnContextClickedMove)
	ON_COMMAND(ID_MODIFICA_ELIMINA, &CMFCOCC01Doc::OnContextClickedDelete)
END_MESSAGE_MAP()


// Costruzione/distruzione di CMFCOCC01Doc

CMFCOCC01Doc::CMFCOCC01Doc() noexcept
{
	// TODO: aggiungere qui il codice di costruzione unico
	m_bAutoDelete = FALSE;
}

CMFCOCC01Doc::~CMFCOCC01Doc()
{
	GetShapeList().clear();
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
	m_hAISContext->DefaultDrawer()->SetWireDraw(Standard_False);
	m_hAISContext->DefaultDrawer()->SetFaceBoundaryDraw(Standard_False);
	//m_hAISContext->DefaultDrawer()->
	//m_hAISContext->DefaultDrawer()->SetUIsoAspect(11);

  //这里设置实体的显示模式
	m_hAISContext->SetDisplayMode(AIS_Shaded, Standard_True);
	m_hAISContext->SetAutomaticHilight(Standard_True);
	return TRUE;
}

void CMFCOCC01Doc::StartSimulation() {
    CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());

}


TopoDS_Shape CMFCOCC01Doc::ConvertAssimpToOpenCASCADE(const aiScene* scene) {
	BRep_Builder builder;
	TopoDS_Compound compound;
	builder.MakeCompound(compound);

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[i];

		CString str;
		str.Format(_T(__FILE__ " >> Conversione... (%i mesh, %i facce)"), scene->mNumMeshes, mesh->mNumFaces);
		SendOutputMessage(str);

		for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
			aiFace& face = mesh->mFaces[j];

			if (face.mNumIndices == 3) { // Assimp guarantees triangles with aiProcess_Triangulate
				gp_Pnt p1(mesh->mVertices[face.mIndices[0]].x, mesh->mVertices[face.mIndices[0]].y, mesh->mVertices[face.mIndices[0]].z);
				gp_Pnt p2(mesh->mVertices[face.mIndices[1]].x, mesh->mVertices[face.mIndices[1]].y, mesh->mVertices[face.mIndices[1]].z);
				gp_Pnt p3(mesh->mVertices[face.mIndices[2]].x, mesh->mVertices[face.mIndices[2]].y, mesh->mVertices[face.mIndices[2]].z);

				TopoDS_Vertex v1 = BRepBuilderAPI_MakeVertex(p1);
				TopoDS_Vertex v2 = BRepBuilderAPI_MakeVertex(p2);
				TopoDS_Vertex v3 = BRepBuilderAPI_MakeVertex(p3);

				TopoDS_Edge e1 = BRepBuilderAPI_MakeEdge(v1, v2);
				TopoDS_Edge e2 = BRepBuilderAPI_MakeEdge(v2, v3);
				TopoDS_Edge e3 = BRepBuilderAPI_MakeEdge(v3, v1);

				TopoDS_Wire wire = BRepBuilderAPI_MakeWire(e1, e2, e3);
				TopoDS_Face face = BRepBuilderAPI_MakeFace(wire);

				builder.Add(compound, face);
			}
		}
	}
	return compound;
}

BOOL CMFCOCC01Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: aggiungere qui il codice di reinizializzazione
	// (nei documenti SDI verrà riutilizzato questo documento).
	InitOCC();

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
	pMainFrame = GetMainFrame();
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

CMFCOCC01Doc* CMFCOCC01Doc::GetDocument() {
	CMDIChildWnd* pChild =
		((CMDIFrameWnd*)(AfxGetApp()->m_pMainWnd))->MDIGetActive();
	if (!pChild)
		return NULL;

	CDocument* pDoc = pChild->GetActiveDocument();
	if (!pDoc)
		return NULL;

	// Check if the document is of the correct type
	if (!pDoc->IsKindOf(RUNTIME_CLASS(CMFCOCC01Doc)))
		return NULL;

	return (CMFCOCC01Doc*)pDoc;
}

void CMFCOCC01Doc::OnOttimizzazioneIniziasimulazione()
{
	COptiDialog optiDialog;
	optiDialog.DoModal();
}


void CMFCOCC01Doc::OnContextClickedMove()
{
	CMFCOCC01Doc* pDoc = GET_ACTIVE_DOC(CMFCOCC01Doc);
	if (pDoc) {
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
				pMFCView->SendMessage(WM_DETECT_COLLISION);
				if (!pDoc->GetSelectedShape().IsNull())
				{
					pDoc->GetSelectedShape()->SetTransparency(0.5);
					pMFCView->SendMessage(WM_REDRAW_VIEW);
				}
			}
		}
	}
}


void CMFCOCC01Doc::OnContextClickedDelete()
{
	CMFCOCC01Doc* pDoc = GET_ACTIVE_DOC(CMFCOCC01Doc);
	if (pDoc) {
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
				pMFCView->SendMessage(WM_DETECT_COLLISION);
				if (!pDoc->GetSelectedShape().IsNull())
				{
					auto& shapeList = pDoc->GetShapeList();
					for (auto it = shapeList.begin(); it != shapeList.end(); )
					{
						if (it->shape == pDoc->GetSelectedShape())
						{
							CString str;
							str.Format(_T( __FILE__ " >> Rimozione %s (0x%p)"), it->name, it->shape);
							pDoc->SendOutputMessage(str);
							pDoc->GetAISContext()->Remove(it->shape, Standard_True);
							pDoc->GetAISContext()->Remove(it->textLabel, Standard_True);
							it = shapeList.erase(it);
							pMFCView->SendMessage(WM_REDRAW_VIEW);
						}
						else
						{
							++it;
						}
					}
				}
			}
		}
	}
}
