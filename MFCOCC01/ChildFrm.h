
// ChildFrm.h: interfaccia della classe CChildFrame
//

#pragma once

class CChildFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame() noexcept;

// Attributi
protected:
	CSplitterWndEx m_wndSplitter;
public:

// Operazioni
public:

// Sostituzioni
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementazione
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Funzioni generate per la mappa dei messaggi
protected:
	DECLARE_MESSAGE_MAP()
};
