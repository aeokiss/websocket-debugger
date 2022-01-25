#pragma once

#ifndef __AFXWIN_H__
	#error "Include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"

class CWebsocketDebuggerApp : public CWinApp
{
public:
	CWebsocketDebuggerApp();

public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CWebsocketDebuggerApp theApp;
