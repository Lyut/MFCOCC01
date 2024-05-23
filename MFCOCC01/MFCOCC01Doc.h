
// MFCOCC01Doc.h: interfaccia della classe CMFCOCC01Doc
//


#pragma once
#include "MainFrm.h"
#include <algorithm>
#include <random>
#include <vector>
#include <set>
#include <tuple>

#include "Node.h"
#include "Packer.h"



class CMFCOCC01Doc : public CDocument
{
protected: // creare solo da serializzazione.
	CMFCOCC01Doc() noexcept;
	DECLARE_DYNCREATE(CMFCOCC01Doc)

// Attributi
public:
	Handle(V3d_Viewer) GetViewer() { return m_hViewer; }
	Handle(AIS_InteractiveContext) GetAISContext() { return m_hAISContext; }
	std::list<Panel>& GetPanelList() { return panelList; }
	CMainFrame* GetMainFrame() { return dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd()); };
// Operazioni
public:
	BOOL InitOCC();
	void StartSimulation();
	void SendOutputMessage(LPCTSTR str, ...);
	void SendInsertItem(LPCTSTR str, ...);
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

// Funzioni generate per la mappa dei messaggi
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Funzione helper che imposta il contenuto della ricerca per il gestore di ricerche
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
private:
	std::list<Panel> panelList;
	Packer packer = Packer(200.0, 200.0);
	Handle(V3d_Viewer) m_hViewer;
	Handle(AIS_InteractiveContext) m_hAISContext;
	CMainFrame* pMainFrame;
};
