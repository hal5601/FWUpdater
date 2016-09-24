#if !defined(AFX_DFUSEDEMODLG_H__7B5D1A5B_CC39_4227_8B3F_DA3BC7AF05B9__INCLUDED_)
#define AFX_DFUSEDEMODLG_H__7B5D1A5B_CC39_4227_8B3F_DA3BC7AF05B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "hexedit.h"
#include "TextProgressCtrl.h"

// CDeviceUpdater dialog

class CDeviceUpdater : public CDialog
{
	DECLARE_DYNAMIC(CDeviceUpdater)

public:
	CDeviceUpdater(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDeviceUpdater();

// Dialog Data
	enum { IDD = IDD_DIALOG_UPDATER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	HICON m_hIcon;
	BOOL OnDeviceChange(UINT nEventType,DWORD dwData);

	// Generated message map functions
	//{{AFX_MSG(CDfuSeDemoDlg)
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedButtondownchoose();
	afx_msg void OnBnClickedButtonupgrade();
	afx_msg void OnBnClickedCancel();

	CTextProgressCtrl	m_Progress;

	void LaunchUpgrade();

	CString			m_DownFileName;
	CString			m_CurrDFUName;
	int				m_CurrentTarget;
	PMAPPING		m_pMapping;
	HANDLE			m_BufferedImage;
	DWORD			m_OperationCode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFUSEDEMODLG_H__7B5D1A5B_CC39_4227_8B3F_DA3BC7AF05B9__INCLUDED_)