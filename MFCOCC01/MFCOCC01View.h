
// MFCOCC01View.h: interfaccia della classe CMFCOCC01View
//

#pragma once
#ifdef USE_IMGUI
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_win32.h"
//#include "imgui/backends/imgui_impl_glfw.h"
#endif


class CMFCOCC01View : public CView, public AIS_ViewController
{
protected: // creare solo da serializzazione.
	CMFCOCC01View() noexcept;
	DECLARE_DYNCREATE(CMFCOCC01View)

// Attributi
public:
	CMFCOCC01Doc* GetDocument() const;

// Operazioni
public:

// Sostituzioni
public:
	virtual void OnDraw(CDC* pDC);  // sottoposto a override per creare questa visualizzazione
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ViewZoom(Standard_Integer iMouseX, Standard_Integer iMouseY, Standard_Real dZoomFactor);
	Standard_Boolean ConvertClickToPoint(Standard_Integer iMouseX, Standard_Integer iMouseY, gp_Pln plnInt, Handle(V3d_View) hView, gp_Pnt& ptResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnInitialUpdate();

// Implementazione
public:
	virtual ~CMFCOCC01View();
	void FitAll();
	TopoDS_Shape ConvertAssimpToOpenCASCADE(const aiScene* scene);
	void DisplayTextOnTopOfModel(const TCollection_AsciiString& text, const gp_Pnt& position);
	gp_Pnt GetShapeCenter(const TopoDS_Shape& shape);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Funzioni generate per la mappa dei messaggi
protected:
	//virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	Handle(V3d_Viewer) m_viewer;
	Handle(AIS_InteractiveContext) m_context;
	Handle(V3d_View) m_view;
	Handle_V3d_View m_hView;
	Standard_Boolean m_bHlrModeIsOn;
	void renderGui();
	std::random_device rd;
	Handle(AIS_Shape) m_selectedBox;
	Quantity_Color m_originalColor;
	Assimp::Importer m_importer;
};

#ifndef _DEBUG  // versione di debug in MFCOCC01View.cpp
inline CMFCOCC01Doc* CMFCOCC01View::GetDocument() const
   { return reinterpret_cast<CMFCOCC01Doc*>(m_pDocument); }
#endif

