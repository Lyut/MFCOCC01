
// MFCOCC01Doc.h: interfaccia della classe CMFCOCC01Doc
//


#pragma once


class CMFCOCC01Doc : public CDocument
{
protected: // creare solo da serializzazione.
	CMFCOCC01Doc() noexcept;
	DECLARE_DYNCREATE(CMFCOCC01Doc)

// Attributi
public:
	Handle(V3d_Viewer) GetViewer() { return m_hViewer; }
	Handle(AIS_InteractiveContext) GetAISContext() { return m_hAISContext; }
	Panel GetMainPanel() { return main_panel; }
	std::list<Panel> GetPanelList() { return panelList; }
	BOOL InitOCC();
	void StartSimulation();
// Operazioni
public:

// Sostituzioni
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementazione
public:
	virtual ~CMFCOCC01Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	gp_Pnt findEmptyPosition(Standard_Real width, Standard_Real height);
	bool panelOverlaps(TopoDS_Shape panel);

// Funzioni generate per la mappa dei messaggi
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Funzione helper che imposta il contenuto della ricerca per il gestore di ricerche
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
private:
	Panel main_panel;
	std::list<Panel> panelList;
	Handle(V3d_Viewer) m_hViewer;
	Handle(AIS_InteractiveContext) m_hAISContext;
};
