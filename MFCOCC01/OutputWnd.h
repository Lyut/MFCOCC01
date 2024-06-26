
#pragma once

/////////////////////////////////////////////////////////////////////////////
// Finestra COutputList

class COutputList : public CListBox
{
// Costruzione
public:
	COutputList() noexcept;

// Implementazione
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CDockablePane
{
// Costruzione
public:
	COutputWnd() noexcept;

	void UpdateFonts();

// Attributi
protected:
	CMFCTabCtrl	m_wndTabs;
	COutputList m_wndOutputBuild;
	COutputList m_wndOutputDebug;
	COutputList m_wndOutputFind;

protected:
	void AdjustHorzScroll(CListBox& wndListBox);

// Implementazione
public:
	virtual ~COutputWnd();
	virtual void AddOutputMessage(LPCTSTR str);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

