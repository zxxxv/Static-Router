#pragma once

#include "pch.h"
#include "ipc2023.h"
#include "afxdialogex.h"
#include "NILayer.h"  // CNILayer 포함
#include "ipc2023Dlg.h"

// RoutingDialog 대화 상자

class RoutingDialog : public CDialogEx
{
	DECLARE_DYNAMIC(RoutingDialog)

public:
	RoutingDialog(CWnd* pParent = nullptr, pcap_if_t** adapterList = nullptr);   // 표준 생성자입니다.
	virtual ~RoutingDialog();
	
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()

private:
	int m_index;
	pcap_if_t** m_pAdapterList;
	CString m_strDeviceName; // 장치 이름을 저장할 멤버 변수

public:
	RoutingList& routingTable = RoutingList::GetInstance();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	BOOL OnInitDialog();
	CComboBox m_comboR;
	CIPAddressCtrl m_destination;
	CIPAddressCtrl m_gateway;
	CIPAddressCtrl m_netmask;
	CButton m_Fup;
	CButton m_Fgateway;
	CButton m_Fhost;
	CEdit m_metric;
};
