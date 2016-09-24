// DeviceUpdater.cpp : implementation file
//
#include "stdafx.h"
#include <dbt.h>
#include "DfuSeDemo.h"
#include "DeviceUpdater.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static GUID	GUID_DFU = { 0x3fe809ab, 0xfb91, 0x4cb5, { 0xa6, 0x43, 0x69, 0x67, 0x0d, 0x52,0x36,0x6e } };
static GUID GUID_APP = { 0xcb979912, 0x5029, 0x420a, { 0xae, 0xb1, 0x34, 0xfc, 0x0a, 0x7d,0x57,0x26 } };

BOOL ReadProtected2 = FALSE;


IMPLEMENT_DYNAMIC(CDeviceUpdater, CDialog)

CDeviceUpdater::CDeviceUpdater(CWnd* pParent /*=NULL*/)
	: CDialog(CDeviceUpdater::IDD, pParent)
{

}

CDeviceUpdater::~CDeviceUpdater()
{
}

void CDeviceUpdater::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDeviceUpdater, CDialog)
	ON_BN_CLICKED(IDC_BUTTONDOWNCHOOSE, &CDeviceUpdater::OnBnClickedButtondownchoose)
	ON_BN_CLICKED(IDC_BUTTONUPGRADE, &CDeviceUpdater::OnBnClickedButtonupgrade)
	ON_BN_CLICKED(IDCANCEL, &CDeviceUpdater::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDeviceUpdater message handlers
BOOL CDeviceUpdater::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	//SetIcon(m_hIcon, FALSE);		// Set small icon

	DEV_BROADCAST_DEVICEINTERFACE filter={0};

	filter.dbcc_size=sizeof(filter);
	filter.dbcc_devicetype=DBT_DEVTYP_DEVICEINTERFACE;
	filter.dbcc_classguid=GUID_DFU;
	RegisterDeviceNotification(m_hWnd, (PVOID)&filter, DEVICE_NOTIFY_WINDOW_HANDLE);
	filter.dbcc_classguid=GUID_APP;
	RegisterDeviceNotification(m_hWnd, (PVOID)&filter, DEVICE_NOTIFY_WINDOW_HANDLE);
	GUID Guid;
	HidD_GetHidGuid(&Guid);
	filter.dbcc_classguid=Guid;
	RegisterDeviceNotification(m_hWnd, (PVOID)&filter, DEVICE_NOTIFY_WINDOW_HANDLE);

	m_Progress.SetShowText(FALSE);
	m_Progress.SetRange(0, 100);
	m_Progress.SetPos(0);
	//SetTimer(1, 300, NULL); // launch the timer that will check operations ongoing

	//Refresh();	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDeviceUpdater::OnBnClickedButtondownchoose()
{
	// TODO: Add your control notification handler code here
	TCHAR szFilters[]=
		"Dfu Files (*.dfu)|*.dfu|All Files (*.*)|*.*||";
	char Path[MAX_PATH];
	char Tmp[MAX_PATH];
	char *pTmp;

	GetModuleFileName(NULL, Path, MAX_PATH);
	strrev(Path);
	pTmp=strchr(Path, '\\');
	strrev(pTmp);
	lstrcpy(Tmp, pTmp);
	lstrcpy(Path, Tmp);
	lstrcat(Path, "*.dfu");

	CFileDialog dlg(TRUE, 
		_T("dfu"), _T("*.dfu"),
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		szFilters, this);
	if (dlg.DoModal()==IDOK)
	{
		WORD Vid, Pid, Bcd;
		BYTE NbTargets;
		HANDLE hFile;

		m_DownFileName=dlg.GetPathName();
		// Get data from chosen .dfu file
		if (STDFUFILES_OpenExistingDFUFile((LPSTR)(LPCSTR)m_DownFileName, &hFile, &Vid, &Pid, &Bcd, &NbTargets)==STDFUFILES_NOERROR)
		{
			BYTE i;
			CString Tempo;
			BOOL bError=FALSE;

			//m_CtrlFileTargets.ResetContent();

			Tempo.Format("%04X", Vid);
			//m_CtrlFileVid.SetWindowText(Tempo);
			Tempo.Format("%04X", Pid);
			//m_CtrlFilePid.SetWindowText(Tempo);
			Tempo.Format("%04X", Bcd);
			//m_CtrlFileBcd.SetWindowText(Tempo);

			for (i=0;i<NbTargets;i++)
			{
				HANDLE Image;
				BYTE Alt;

				if (STDFUFILES_ReadImageFromDFUFile(hFile, i, &Image)==STDFUFILES_NOERROR)
				{
					if (STDFUFILES_GetImageAlternate(Image, &Alt)==STDFUFILES_NOERROR)
					{
						char Name[512]={0};

						STDFUFILES_GetImageName(Image, Name);
						Tempo.Format("%02i\t%s", Alt, Name);

						//m_CtrlFileTargets.AddString(Tempo);
					}
					else
					{
						MessageBox("Unable to get data from image...");
						bError=TRUE;
						break;
					}

					STDFUFILES_DestroyImage(&Image);
				}
				else
				{
					MessageBox("Bad file format...");
					bError=TRUE;
					break;
				}
			}
			STDFUFILES_CloseDFUFile(hFile);
			//m_DownFile.SetWindowText(dlg.GetFileName());

			if (!bError)
			{
				MessageBox("File correctly loaded.");
			}
		}
		else
			MessageBox("This file doesn't have a correct format...");
	}
}

void CDeviceUpdater::OnBnClickedButtonupgrade()
{
	// TODO: Add your control notification handler code here
	if(ReadProtected2)
	{
		    if (AfxMessageBox("Your device is read protected.\nWould you remove the read protection?", MB_YESNO |MB_ICONQUESTION)==IDYES)
			{
				HANDLE hdl;
                if (STDFU_Open((LPSTR)(LPCSTR)m_CurrDFUName,&hdl)==STDFU_NOERROR)
				{
				   if (STDFU_SelectCurrentConfiguration(&hdl, 0, 0,1)==STDFU_NOERROR)
				   {
					   DFUSTATUS DFUStatus;

					   STDFU_Getstatus(&hdl, &DFUStatus);
					   while(DFUStatus.bState != STATE_DFU_IDLE)
					   {
							STDFU_Clrstatus(&hdl);
							STDFU_Getstatus(&hdl, &DFUStatus);
					   }
						 
					   LPBYTE m_pBuffer = (LPBYTE)malloc(0x10);
					   memset(m_pBuffer, 0xFF, 0x10);
					   m_pBuffer[0] = 0x92;

					   STDFU_Dnload(&hdl, m_pBuffer, 0x01, 0); 

					   STDFU_Getstatus(&hdl, &DFUStatus);
				   }
				   STDFU_Close(&hdl);
				   ReadProtected2 = FALSE;
				}

			}
			else 
				return;
	}


	CString Tempo, FileId, DevId;

	UpdateData(TRUE);
	
	LaunchUpgrade();

	UpdateData(FALSE);
}

void CDeviceUpdater::LaunchUpgrade() 
{
	DFUThreadContext Context;
	HANDLE hFile;
	BYTE NbTargets;
	BOOL bFound=FALSE;
	DWORD dwRet;
	int i, TargetSel=m_CurrentTarget;
	HANDLE hImage;



	// Get the image of the selected target
	dwRet=STDFUFILES_OpenExistingDFUFile((LPSTR)(LPCSTR)m_DownFileName, &hFile, NULL, NULL, NULL, &NbTargets);
	if (dwRet==STDFUFILES_NOERROR)
	{
		for (i=0;i<NbTargets;i++)
		{
			HANDLE Image;
			BYTE Alt;

			if (STDFUFILES_ReadImageFromDFUFile(hFile, i, &Image)==STDFUFILES_NOERROR)
			{
				if (STDFUFILES_GetImageAlternate(Image, &Alt)==STDFUFILES_NOERROR)
				{
					if (Alt==TargetSel)
					{
						hImage=Image;
						bFound=TRUE;
						break;
					}
				}
				STDFUFILES_DestroyImage(&Image);
			}
		}
		STDFUFILES_CloseDFUFile(hFile);
	}
	else
	{
		Context.ErrorCode=dwRet;
		//HandleError(&Context);
	}

	if (!bFound)
	{
		MessageBox("Unable to find data for that device/target from the file ...");
		return;
	}
	else
	{
		BOOL m_TransferOptimized = FALSE;
		// prepare the asynchronous operation: first is erase !
		lstrcpy(Context.szDevLink, m_CurrDFUName);
		Context.DfuGUID=GUID_DFU;
		Context.AppGUID=GUID_APP;
		Context.Operation=OPERATION_ERASE;
		Context.bDontSendFFTransfersForUpgrade=m_TransferOptimized;
		if (m_BufferedImage)
			STDFUFILES_DestroyImage(&m_BufferedImage);
		// Let's backup our data before the filtering for erase. The data will be used for the upgrade phase
		STDFUFILES_DuplicateImage(hImage, &m_BufferedImage);
		STDFUFILES_FilterImageForOperation(m_BufferedImage, m_pMapping+TargetSel, OPERATION_UPGRADE, m_TransferOptimized);

		STDFUFILES_FilterImageForOperation(hImage, m_pMapping+TargetSel, OPERATION_ERASE, m_TransferOptimized);
		Context.hImage=hImage;

		//UpdateData(TRUE);
		//m_DataSize.Format("0 KB(0 Bytes) of %i KB(%i Bytes)", STDFUFILES_GetImageSize(m_BufferedImage)/1024,  STDFUFILES_GetImageSize(m_BufferedImage));
		//UpdateData(FALSE);

		//startTime = CTime::GetCurrentTime();

		dwRet=STDFUPRT_LaunchOperation(&Context, &m_OperationCode);
		if (dwRet!=STDFUPRT_NOERROR)
		{
			Context.ErrorCode=dwRet;
			//HandleError(&Context);
		}
		else
		{
			m_Progress.SetPos(0);
			m_Progress.SetWindowText("");
			m_Progress.SetShowText(TRUE);
		}
	}
}

void CDeviceUpdater::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
