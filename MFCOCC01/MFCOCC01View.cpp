
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
#ifdef USE_IMGUI
#include <backends/imgui_impl_win32.cpp>
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
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_REDRAW_VIEW, &CMFCOCC01View::OnRedrawView)
	ON_MESSAGE(WM_DETECT_COLLISION, &CMFCOCC01View::DetectCollision)
	ON_MESSAGE(WM_FITALL, &CMFCOCC01View::FitAllMsg)
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

gp_Pnt CMFCOCC01View::GetShapeCenter(const TopoDS_Shape& shape) {
	Bnd_Box boundingBox;
	BRepBndLib::Add(shape, boundingBox);

	Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
	boundingBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);

	gp_Pnt center((xmin + xmax) / 2, (ymin + ymax) / 2, (zmin + zmax) / 2);
	return center;
}

LRESULT CMFCOCC01View::OnRedrawView(WPARAM wParam, LPARAM lParam)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CMFCOCC01Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return 0;
	Handle(AIS_InteractiveContext) context = pDoc->GetAISContext();
	context->UpdateCurrentViewer();
	for (objList obj : GetDocument()->GetShapeList()) {
		Quantity_Color textColor(Quantity_NOC_BLACK);
		Quantity_Color color(obj.color);

		obj.shape->SetColor(color);
		context->Display(obj.shape, Standard_True);

		gp_Pnt shapeCenter = GetShapeCenter(obj.topo_shape);
		shapeCenter.SetY(shapeCenter.Y() * 2.3);
		shapeCenter.SetZ(shapeCenter.Z() * 2);
		shapeCenter.SetX(shapeCenter.X() * 2);
		Handle(AIS_TextLabel) aTextLabel = new AIS_TextLabel();
		CString str;
		str.Format(_T("%s (0x%p)"), obj.name, obj.shape);
		aTextLabel->SetText(TCollection_ExtendedString(str));
		aTextLabel->SetPosition(shapeCenter);
		aTextLabel->SetColor(textColor);
		aTextLabel->SetFont("RomanS");
		aTextLabel->SetDisplayType(Aspect_TODT_DEKALE);

		context->Display(aTextLabel, Standard_True);

	}
	return 0;
}

// Disegno di CMFCOCC01View
void CMFCOCC01View::OnDraw(CDC* pDC)
{
	CMFCOCC01Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Handle(AIS_InteractiveContext) context = pDoc->GetAISContext();

	if (!m_selectedBox.IsNull())
	{
		m_selectedBox->SetColor(Quantity_NOC_RED);
		GetDocument()->GetAISContext()->Redisplay(m_selectedBox, true);
	}

	context->UpdateCurrentViewer();
#ifdef USE_IMGUI
	renderGui();
#endif

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
	m_hView->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_BLACK, 0.07, V3d_ZBUFFER);

	Handle(Prs3d_Drawer) highlightStyle = GetDocument()->GetAISContext()->HighlightStyle();
	highlightStyle->SetMethod(Aspect_TOHM_COLOR);
	highlightStyle->SetColor(Quantity_NOC_ANTIQUEWHITE); // Set to black or transparent
	highlightStyle->SetTransparency(1.0); // Fully transparent
	GetDocument()->GetAISContext()->SetHighlightStyle(Prs3d_TypeOfHighlight_Selected, highlightStyle);

	m_hView->SetProj(V3d_Zpos);
	FitAll();
	m_hView->Redraw();
	//FitAll();

#ifdef USE_IMGUI
	/* IMGUI */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(m_hWnd);
    ImGui_ImplOpenGL3_Init("#version 130");
#endif
	
}

void CMFCOCC01View::FitAll()
{
	if (!m_hView.IsNull())
		m_hView->FitAll();
	m_hView->ZFitAll();
}

LRESULT CMFCOCC01View::FitAllMsg(WPARAM wParam, LPARAM lParam)
{
	FitAll();
	return 0;
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

void CMFCOCC01View::OnLButtonUp(UINT /* nFlags */, CPoint point)
{
	CMFCOCC01Doc* pDoc = GetDocument();
	if (pDoc && pDoc->GetViewer()) {
		Handle(AIS_InteractiveContext) context = pDoc->GetAISContext();
		context->UpdateCurrentViewer();
	}
	 //Invalidate();
}

LRESULT CMFCOCC01View::DetectCollision(WPARAM wParam, LPARAM lParam)
{
	if (!m_selectedBox.IsNull())
	{
		CMFCOCC01Doc* pDoc = GET_ACTIVE_DOC(CMFCOCC01Doc);
		if (pDoc)
			pDoc->SetSelectedShape(m_selectedBox);
	}

	return 0;
}

void CMFCOCC01View::OnLButtonDown(UINT nFlags, CPoint point)
{

	m_lastMousePos = point;
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	GLdouble modelview[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	GLfloat winX = static_cast<GLfloat>(point.x);
	GLfloat winY = static_cast<GLfloat>(viewport[3] - point.y);

	GLdouble posX, posY, posZ;
	glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &posZ);
	gluUnProject(winX, winY, posZ, modelview, projection, viewport, &posX, &posY, &posZ);
	gp_Pnt clickPoint(posX, posY, posZ);

	Handle(AIS_InteractiveContext) context = GetDocument()->GetAISContext();
	context->MoveTo(point.x, point.y, m_hView, true);
	if (context->HasDetected())
	{
		Handle(AIS_InteractiveObject) detected = context->DetectedInteractive();
		if (!detected.IsNull() && detected->IsKind(STANDARD_TYPE(AIS_Shape)))
		{
			Handle(AIS_Shape) detectedShape = Handle(AIS_Shape)::DownCast(detected);

			// Restore the original color of the previously selected panel
			if (!m_selectedBox.IsNull())
			{
				for (objList& obj : GetDocument()->GetShapeList())
				{
					if (obj.shape == m_selectedBox)
					{
						m_selectedBox->SetColor(obj.originalColor);
						context->Redisplay(m_selectedBox, true);
						break;
					}
				} 
			}

			// Update m_selectedBox and set the new selection color
			m_selectedBox = detectedShape;

			// Find the corresponding panel for the newly selected box and update its color
			for (objList& obj : GetDocument()->GetShapeList())
			{
				if (obj.shape == m_selectedBox)
				{
					m_originalColor = obj.color;  // Store the panel's original color
					m_selectedBox->SetColor(Quantity_NOC_RED);
					break;
				}
			} 

			context->Redisplay(m_selectedBox, true);
		}
	}
	else
	{
		for (objList& obj : GetDocument()->GetShapeList())
		{
				obj.color = obj.originalColor;
				obj.shape->SetColor(obj.color);
				context->Redisplay(obj.shape, true);
		}
	}

	context->UpdateCurrentViewer();

	CView::OnLButtonDown(nFlags, point);
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
	FlushViewEvents(m_context, m_hView, Standard_True);
    CView::OnMouseMove(nFlags, point);
	if (nFlags & MK_LBUTTON) {
		int dx = point.x - m_lastMousePos.x;
		int dy = point.y - m_lastMousePos.y;

		m_hView->Rotation(dx, dy);
		CMFCOCC01Doc* pDoc = GetDocument();
		Handle(AIS_InteractiveContext) context = pDoc->GetAISContext();
		context->UpdateCurrentViewer();

	}

#ifdef USE_IMGUI
	HDC hdc = ::GetDC(m_hWnd);
	SwapBuffers(hdc);
	renderGui();
	::ReleaseDC(m_hWnd, hdc);
#endif 
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

void CMFCOCC01View::renderGui()
{
#ifdef USE_IMGUI
	ImGuiIO& aIO = ImGui::GetIO();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowMetricsWindow();
	//ImGui::ShowDemoWindow();

	ImGui::Begin("Lutron 0.0.6 Build " __DATE__ " " __TIME__);
	ImGui::Text("Menù debug");
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::TreeNode("Pannelli/tagli"))
	{
		if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("lista"))
			{
				for (const auto& panel : GetDocument()->GetPanelList()) {
					ImGui::BulletText("pannello in (%.1f, %.1f, %.1f)", panel.origin.X(), panel.origin.Y(), panel.origin.Z());
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("aggiungi pann."))
			{
				static int i0 = 15;
				ImGui::InputInt("alt.", &i0);
				static int i1 = 10;
				ImGui::InputInt("larg.", &i1);
				if (ImGui::Button("aggiungi")) {
					auto newPanel = std::make_shared<Panel>();
					newPanel->origin = gp_Pnt();
					newPanel->height = i0;
					newPanel->width = i1;
					newPanel->thickness = 10;
					std::mt19937 gen(rd());
					std::uniform_int_distribution<int> dis(1, 508);
					int randomIndex = dis(gen);
					newPanel->color = static_cast<Quantity_NameOfColor>(randomIndex);

					GetDocument()->GetPanelList().push_back(*newPanel);
					GetDocument()->StartSimulation();

					//HDC hdc = ::GetDC(m_hWnd);
					//UpdateWindow();
					//m_hView->Redraw();
					//FlushViewEvents(m_context, m_hView, Standard_True);
					//::ReleaseDC(m_hWnd, hdc);
					Invalidate();
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
			ImGui::TreePop();
		}
	}
	
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
}

BOOL CMFCOCC01View::PreTranslateMessage(MSG* pMsg)
{
#ifdef USE_IMGUI
	if (ImGui_ImplWin32_WndProcHandler(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam))
		return true;
#endif
	return CView::PreTranslateMessage(pMsg);
}