#pragma once

#include "pch.h"
#include "ipc2023.h"
#include "afxdialogex.h"
#include "NILayer.h"  // CNILayer 포함
#include "ipc2023Dlg.h"

// ProxyDialog 대화 상자

class ProxyDialog : public CDialogEx
{
    DECLARE_DYNAMIC(ProxyDialog)

public:
    ProxyDialog(CWnd* pParent = nullptr, pcap_if_t** adapterList = nullptr);   // 표준 생성자입니다.
    virtual ~ProxyDialog();

    // 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG1 };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV 지원입니다.
    DECLARE_MESSAGE_MAP()

private:
    // CNILayer 포인터 추가
    // CNILayer* m_NI;
    int m_index; // 선택된 어댑터의 인덱스
    CString pSrcEdit; // 소스 주소를 저장할 변수
    pcap_if_t** m_pAdapterList;//

public:
    CString m_strIPAddress; // IP 주소를 저장할 멤버 변수
    CString m_strMACAddress; // MAC 주소를 저장할 멤버 변수
    CString m_strDeviceName; // 장치 이름을 저장할 멤버 변수
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    BOOL OnInitDialog();
    CComboBox m_combop;
    CIPAddressCtrl m_ipaddressp;
    CEdit m_editp;
};
