
// ipc2023Dlg.h: 헤더 파일
//

#pragma once

#include "LayerManager.h"	// Added by ClassView
#include "EthernetLayer.h"	// Added by ClassView
#include "NILayer.h"
#include "IPLayer.h"
#include "ARPProxyTable.h"

// Cipc2023Dlg 대화 상자
class Cipc2023Dlg : public CDialogEx, public CBaseLayer
{
// 생성입니다.
public:
	Cipc2023Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.



// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IPC2023_DIALOG };
#endif

	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:


public:
	BOOL			Receive(unsigned char* ppayload);
	void UpdateListCtrlItem(const CString& ip, const CString& mac, const CString& status); // 캐시 테이블 변경
	void TimeoutEntryDelete(const unsigned char* ip);

private:
	CLayerManager	m_LayerMgr;
	int				m_nAckReady;

	enum {
		IPC_INITIALIZING,
		IPC_READYTOSEND,
		IPC_WAITFORACK,
		IPC_ERROR,
		IPC_COMBO_SET
	};

	void			SetDlgState(int state);
	inline void		EndofProcess();
	void Str2UCHAR(CString& src, UCHAR* dst);
	void UCHAR2Str(UCHAR* src, CString& dst);

	BOOL			m_bSendReady;

	// Implementation
	UINT			m_wParam;
	DWORD			m_lParam;

	// Object Layer
	CNILayer* m_NI;
	CEthernetLayer* m_Eth;
	CIPLayer* m_IP;

	//CString Cipc2023Dlg::binaryToString(const unsigned char* ip);

public:
	CString m_iMacSrc;
	CString m_oMacSrc;
	UCHAR m_ucSrcAddrArray[6];
	UCHAR m_ucGaprSrcAddrArray[6];
	UCHAR m_ucDstAddrArray[6];
	UCHAR m_unused[100];
	ARPProxyTable& proxyTable = ARPProxyTable::GetInstance();
	int m_index;
	afx_msg void OnBnClickedButtonDelete();
	CListCtrl m_ListCtrl;		// ARP 캐시 테이블
	CIPAddressCtrl m_ipSource;	// 내 IP 주소
	CIPAddressCtrl m_ipTarget;	// 상대 IP 주소
	afx_msg void OnBnClickedArpTable();
	CListCtrl m_ListCtrlP;		// 프록시 테이블
	afx_msg void OnBnClickedProxyAdd();
	afx_msg void OnBnClickedProxyDelete();
	CEdit m_garp_mac;			// 변경한 mac 주소
	afx_msg void OnBnClickedProxyTable();
	CListCtrl m_ListCtrlR;
	afx_msg void OnBnClickedButtonEnd();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnCbnSelchangeComboMac();
	afx_msg void OnCbnSelchangeComboMac2();

	CComboBox m_comboBox1;
	CComboBox m_comboBox2;
	afx_msg void OnBnClickedButtonRadd();
	CIPAddressCtrl m_ip1;
	CIPAddressCtrl m_ip2;
	afx_msg void OnBnClickedButtonRdelete();

	typedef struct _INTERFACE {
		
		unsigned char macAddr[6];
		unsigned char ipAddr[4];

	} INTERFACE;
};
