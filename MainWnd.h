#if !defined(AFX_MAINWND_H__7638CFA6_8601_4E4D_9006_B4DDE63FDF1A__INCLUDED_)
#define AFX_MAINWND_H__7638CFA6_8601_4E4D_9006_B4DDE63FDF1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMainWnd frame

class CMainWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CMainWnd)

// Attributes
public:

// Operations
public:
	void WormExe(char szHostFile[]);
	CMainWnd();
	BOOL CreateFrame();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMainWnd();

	// Generated message map functions
	//{{AFX_MSG(CMainWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINWND_H__7638CFA6_8601_4E4D_9006_B4DDE63FDF1A__INCLUDED_)
