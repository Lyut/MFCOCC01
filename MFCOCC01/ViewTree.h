
#pragma once

/////////////////////////////////////////////////////////////////////////////
// Finestra CViewTree

class CViewTree : public CTreeCtrl
{
// Costruzione
public:
	CViewTree() noexcept;

// Sostituzioni
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// Implementazione
public:
	virtual ~CViewTree();

protected:
	DECLARE_MESSAGE_MAP()
};
