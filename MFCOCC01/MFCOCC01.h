
// MFCOCC01.h: file di intestazione principale per l'applicazione MFCOCC01
//
#pragma once

#ifndef __AFXWIN_H__
	#error "includere 'pch.h' prima di includere questo file per PCH"
#endif

#include "resource.h"       // simboli principali



// CMFCOCC01App:
// Per l'implementazione di questa classe, vedere MFCOCC01.cpp
//

class CMFCOCC01App : public CWinAppEx
{
public:
	CMFCOCC01App() noexcept;
	Handle_Graphic3d_GraphicDriver GetGraphicDriver() const;
private:
	Handle_Graphic3d_GraphicDriver m_hGraphicDriver;


// Override
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementazione

	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCOCC01App theApp;
