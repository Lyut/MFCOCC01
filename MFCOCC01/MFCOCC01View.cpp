
// MFCOCC01View.cpp: implementazione della classe CMFCOCC01View
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCOCC01View

IMPLEMENT_DYNCREATE(CMFCOCC01View, CView)

BEGIN_MESSAGE_MAP(CMFCOCC01View, CView)
	// Comandi di stampa standard
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMFCOCC01View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// Costruzione/distruzione di CMFCOCC01View

CMFCOCC01View::CMFCOCC01View() noexcept
{
	// TODO: aggiungere qui il codice di costruzione.

}

CMFCOCC01View::~CMFCOCC01View()
{
}

BOOL CMFCOCC01View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: modificare la classe o gli stili Window modificando
	//  la struttura CREATESTRUCT

	return CView::PreCreateWindow(cs);

}

// Disegno di CMFCOCC01View

void CMFCOCC01View::OnDraw(CDC* pDC)
{
	CMFCOCC01Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Handle(AIS_InteractiveContext) context = pDoc->GetAISContext();
	context->SetDisplayMode(AIS_Shaded, true);

	BRepPrimAPI_MakeBox mkBox(gp_Pnt(0, 0, 0), 300.0, 100.0, 2.0);
	TopoDS_Shape Box = mkBox.Shape();
	Handle(AIS_Shape) myAISBox = new AIS_Shape(Box);

	GetDocument()->GetAISContext()->Display(myAISBox, Standard_False);

	for (Panel& panel : pDoc->GetPanelList()) {
		BRepPrimAPI_MakeBox mkBox(panel.origin, panel.height, panel.width, panel.thickness);
		TopoDS_Shape Box = mkBox.Shape();
		Handle(AIS_Shape) myAISBox = new AIS_Shape(Box);
		Quantity_Color color(panel.color);
		myAISBox->SetColor(color);

		GetDocument()->GetAISContext()->Display(myAISBox, Standard_True);
	}

	context->UpdateCurrentViewer();


	// Start the ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ImGui content
	ImGui::Begin("OpenCascade Panels");
	ImGui::Text("Panels Configuration:");
	for (const auto& panel : pDoc->GetPanelList()) {
		ImGui::Text("Panel at (%.1f, %.1f, %.1f)", panel.origin.X(), panel.origin.Y(), panel.origin.Z());
	}
	ImGui::End();

	// Render ImGui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


	SwapBuffers(pDC->m_hDC);

	// TODO: aggiungere qui il codice di disegno per i dati nativi.
}


void CMFCOCC01View::OnInitialUpdate()
{

	CView::OnInitialUpdate();
	m_bHlrModeIsOn = Standard_False;
	m_hView = GetDocument()->GetViewer()->CreateView();
	m_hView->SetComputedMode(m_bHlrModeIsOn);
	Handle(Graphic3d_GraphicDriver) graphicDriver = ((CMFCOCC01App*)AfxGetApp())->GetGraphicDriver();

	CRect clientRect;
	GetClientRect(&clientRect);
	int width = clientRect.Width();
	int height = clientRect.Height();

	Handle(WNT_Window) hWntWindow = new WNT_Window(GetSafeHwnd());
	if (!hWntWindow->IsMapped()) {
		hWntWindow->Map();
	}

	hWntWindow->Size(width, height);

	m_hView->SetWindow(hWntWindow);
	m_hView->ZBufferTriedronSetup(Quantity_NOC_RED, Quantity_NOC_GREEN, Quantity_NOC_BLUE1, 0.8, 0.05, 12);
	m_hView->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.1, V3d_ZBUFFER);

	FitAll();

	/* IMGUI */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(m_hWnd);
    ImGui_ImplOpenGL3_Init("#version 130");
	
}

void CMFCOCC01View::FitAll()
{
	if (!m_hView.IsNull())
		m_hView->FitAll();
	m_hView->ZFitAll();
}



// Stampa di CMFCOCC01View


void CMFCOCC01View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMFCOCC01View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// preparazione predefinita
	return DoPreparePrinting(pInfo);
}

void CMFCOCC01View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: inserire un'ulteriore inizializzazione prima della stampa.
}

void CMFCOCC01View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: aggiungere pulizia dopo la stampa.
}

void CMFCOCC01View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMFCOCC01View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

void CMFCOCC01View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (!m_hView.IsNull())
	{
		m_hView->MustBeResized();
		m_hView->Redraw();
	}

}

void CMFCOCC01View::OnMouseMove(UINT nFlags, CPoint point)
{

	CView::OnMouseMove(nFlags, point);
	if (nFlags && MK_LBUTTON) {
		//myView->Rotate(point.x,point.y); 
		m_hView->Rotation(point.x, point.y);
	}
}

Standard_Boolean CMFCOCC01View::ConvertClickToPoint(Standard_Integer iMouseX, Standard_Integer iMouseY, gp_Pln plnInt, Handle(V3d_View) hView, gp_Pnt& ptResult)
{
	Standard_Real rEyeX, rEyeY, rEyeZ;
	hView->Eye(rEyeX, rEyeY, rEyeZ);
	gp_Pnt ptEye(rEyeX, rEyeY, rEyeZ);

	Standard_Real rProjX, rProjY, rProjZ;
	hView->Proj(rProjX, rProjY, rProjZ);

	Standard_Real rFarX, rFarY, rFarZ;
	hView->Convert(iMouseX, iMouseY, rFarX, rFarY, rFarZ);
	gp_Pnt ptFar(rFarX, rFarY, rFarZ);

	Handle(Geom_Line) pGeomLine;
	switch (hView->Type())
	{
	case V3d_ORTHOGRAPHIC:
		pGeomLine = new Geom_Line(ptFar, gp_Dir(rProjX, rProjY, rProjZ));
		break;
	case V3d_PERSPECTIVE:
		pGeomLine = new Geom_Line(ptEye, gp_Dir(gp_Vec(ptEye, ptFar)));
		break;
	default:
		return Standard_False;
	}

	Handle(Geom_Plane) pGeomPlane = new Geom_Plane(plnInt);
	GeomAPI_IntCS intCS(pGeomLine, pGeomPlane);
	if (intCS.IsDone() && intCS.NbPoints() == 1)
	{
		ptResult = intCS.Point(1);
		return Standard_True;
	}
	else
		return Standard_False;
}


void CMFCOCC01View::ViewZoom(Standard_Integer iMouseX, Standard_Integer iMouseY, Standard_Real dZoomFactor)
{
	// Calculate zoom coefficient
	Standard_Real rZoomCoef = dZoomFactor;
	if (m_hView->Type() == V3d_PERSPECTIVE)
		if (rZoomCoef < 1.)
		{
			Standard_Real rDelta = 1 / rZoomCoef - 1;
			rZoomCoef = (1 + rDelta) / (1 + 2 * rDelta);
		}


	// Calculate zoom point
	Standard_Real rAtX, rAtY, rAtZ;
	m_hView->At(rAtX, rAtY, rAtZ);
	gp_Pnt ptAt(rAtX, rAtY, rAtZ);

	Standard_Real rProjX, rProjY, rProjZ;
	m_hView->Proj(rProjX, rProjY, rProjZ);
	gp_Dir dirProj(rProjX, rProjY, rProjZ);

	gp_Pln plnView(ptAt, dirProj);
	gp_Pnt ptMouse;
	ConvertClickToPoint(iMouseX, iMouseY, plnView, m_hView, ptMouse);
	gp_Vec vecMouse(ptAt, ptMouse);


	// Calculate panning distance
	gp_Pnt ptOrg;
	ConvertClickToPoint(0, 0, plnView, m_hView, ptOrg);
	gp_Pnt ptX;
	ConvertClickToPoint(1, 0, plnView, m_hView, ptX);
	gp_Pnt ptY;
	ConvertClickToPoint(0, 1, plnView, m_hView, ptY);
	gp_Dir dirX(gp_Vec(ptOrg, ptX));
	gp_Dir dirY(gp_Vec(ptY, ptOrg));

	Standard_Real rDX = vecMouse.Dot(dirX);
	Standard_Real rDY = vecMouse.Dot(dirY);

	Standard_Real rPixel2MM = m_hView->Convert(1);
	Standard_Integer iDX = (Standard_Integer)Round(rDX / rPixel2MM);
	Standard_Integer iDY = (Standard_Integer)Round(rDY / rPixel2MM);

	Standard_Real rPanningX = iDX * rPixel2MM * (1. / rZoomCoef - 1.);
	Standard_Real rPanningY = iDY * rPixel2MM * (1. / rZoomCoef - 1.);


	// Zoom
	Standard_Real rOldDepth = m_hView->Depth();
	m_hView->Panning(rPanningX, rPanningY);
	m_hView->SetZoom(rZoomCoef);

	Standard_Real rOffset = m_hView->Depth() - rOldDepth;
	m_hView->At(rAtX, rAtY, rAtZ);
	rAtX += rOffset * rProjX;
	rAtY += rOffset * rProjY;
	rAtZ += rOffset * rProjZ;
	m_hView->SetAt(rAtX, rAtY, rAtZ);
}

BOOL CMFCOCC01View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	CMFCOCC01Doc* pDoc = GetDocument();
	if (pDoc && pDoc->GetViewer())
	{
		CPoint ptClient;
		ScreenToClient(&ptClient);
		ptClient += pt;

		double dStep = zDelta / 120. / 20.;
		double dZoomFactor = 0.;
		if (zDelta > 0.)
			dZoomFactor = 1. + dStep;
		else
			dZoomFactor = 1. / (1. - dStep);

		if (dZoomFactor > 0.)
		{
			ViewZoom(ptClient.x, ptClient.y, dZoomFactor);
			Handle(AIS_InteractiveContext) context = pDoc->GetAISContext();
			context->UpdateCurrentViewer();
		}
		else
			ASSERT(0);
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}



// Diagnostica di CMFCOCC01View

#ifdef _DEBUG
void CMFCOCC01View::AssertValid() const
{
	CView::AssertValid();
}

void CMFCOCC01View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCOCC01Doc* CMFCOCC01View::GetDocument() const // la versione non debug è inline.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCOCC01Doc)));
	return (CMFCOCC01Doc*)m_pDocument;
}
#endif //_DEBUG


// Gestori di messaggi di CMFCOCC01View