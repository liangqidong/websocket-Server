// PigYear.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PigYear.h"
#include "MainWnd.h"//----------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPigYearApp

BEGIN_MESSAGE_MAP(CPigYearApp, CWinApp)
	//{{AFX_MSG_MAP(CPigYearApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPigYearApp construction

CPigYearApp::CPigYearApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPigYearApp object

CPigYearApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPigYearApp initialization

BOOL CPigYearApp::InitInstance()
{
	CMainWnd* pWnd=new CMainWnd();
	pWnd->CreateFrame();
	m_pMainWnd=pWnd;
	return TRUE;// 返回TRUE，开始消息循环。 
}
