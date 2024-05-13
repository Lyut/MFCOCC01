
// ChildFrm.cpp: implementazione della classe CChildFrame
//

#include "pch.h"
#include "framework.h"
#include "MFCOCC01.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
END_MESSAGE_MAP()

// Costruzione/distruzione di CChildFrame

CChildFrame::CChildFrame() noexcept
{
	// TODO: aggiungere qui il codice di inizializzazione dei membri.
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: modificare qui la classe o gli stili Window modificando la struttura CREATESTRUCT
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// Diagnostica di CChildFrame

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// Gestori di messaggi di CChildFrame
