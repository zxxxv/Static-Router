// RoutingDialog.cpp: 구현 파일
//

#include "pch.h"
#include "ipc2023.h"
#include "afxdialogex.h"
#include "RoutingDialog.h"


// RoutingDialog 대화 상자

IMPLEMENT_DYNAMIC(RoutingDialog, CDialogEx)

RoutingDialog::RoutingDialog(CWnd* pParent /*=nullptr*/, pcap_if_t** adapterList)
	: CDialogEx(IDD_DIALOG2, pParent), m_index(0)
{
    m_pAdapterList = adapterList;

}

RoutingDialog::~RoutingDialog()
{
}

BOOL RoutingDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    UpdateData(TRUE); // OnInitDialog에서는 필요하지 않음
    CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBOR);
    if (pComboBox)
    {
        for (int i = 0; i < NI_COUNT_NIC; ++i) {
            pcap_if_t* tempAdapter = m_pAdapterList[i];
            pComboBox->AddString(tempAdapter->description);
            pComboBox->SetCurSel(0); // 첫 번째 항목 선택
            if (!tempAdapter->next) break;
        }
    }
    UpdateData(FALSE);
    return TRUE; // 대화 상자가 포커스를 설정하지 않으면 TRUE를 반환합니다.
}

void RoutingDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBOR, m_comboR);
    DDX_Control(pDX, IDC_IPADDRESSRD, m_destination);
    DDX_Control(pDX, IDC_IPADDRESSRG, m_gateway);
    DDX_Control(pDX, IDC_IPADDRESSRN, m_netmask);
    DDX_Control(pDX, IDC_CHECK1, m_Fup);
    DDX_Control(pDX, IDC_CHECK2, m_Fgateway);
    DDX_Control(pDX, IDC_EDIT_RM, m_metric);
}


BEGIN_MESSAGE_MAP(RoutingDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &RoutingDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &RoutingDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// RoutingDialog 메시지 처리기


void RoutingDialog::OnBnClickedOk()
{
    // destination, gateway, netmask를 unsigned char[4] 형태로 가져오기
    unsigned char dst[4] = { 0 };
    unsigned char gate[4] = { 0 };
    unsigned char mask[4] = { 0 };

    m_destination.GetAddress(dst[0], dst[1], dst[2], dst[3]);
    m_gateway.GetAddress(gate[0], gate[1], gate[2], gate[3]);
    m_netmask.GetAddress(mask[0], mask[1], mask[2], mask[3]);

    // 플래그 설정 (체크 박스 상태를 기반으로 플래그 결정)
    e_flag flag = e_flag::none;

    // 체크박스 상태를 확인하여 플래그 설정
    // Up만 체크
    if (m_Fup.GetCheck() == BST_CHECKED) {
        flag = e_flag::up;
    }
    // Up과 Gateway 체크
    if (m_Fup.GetCheck() == BST_CHECKED && m_Fgateway.GetCheck() == BST_CHECKED) {
        flag = e_flag::gateway;
    }

    // 선택한 네트워크 디바이스 이름 가져오기
    CString strDeviceName;
    m_comboR.GetLBText(m_comboR.GetCurSel(), strDeviceName);
    if (strDeviceName.IsEmpty()) {
        AfxMessageBox(_T("Interface를 선택해 주세요"));
        return;
    }

    // 선택된 네트워크 디바이스의 인덱스를 찾아 interfaceFlag로 설정
    unsigned short interfaceFlag = 0; // 디바이스를 식별할 수 있는 인덱스 혹은 플래그
    bool foundDevice = false; // 디바이스가 발견되었는지 확인하는 플래그

    // m_pAdapterList를 사용하여 선택된 디바이스의 인덱스를 검색
    for (int i = 0; i < NI_COUNT_NIC; ++i) {
        if (m_pAdapterList[i] && strDeviceName == m_pAdapterList[i]->description) {
            interfaceFlag = static_cast<unsigned short>(i);
            foundDevice = true;
            break;
        }
    }

    if (!foundDevice) {
        AfxMessageBox(_T("해당 Device가 없습니다"));
        return;
    }

    // 메트릭 값
    CString strMetric;
    m_metric.GetWindowText(strMetric); // 메트릭 값 가져오기
    unsigned short metricValue = static_cast<unsigned short>(_ttoi(strMetric)); // CString을 정수형으로 변환

    // 메트릭 값이 유효한지 검사 (예: 0보다 큰 값이어야 함)
    if (metricValue <= 0) {
        AfxMessageBox(_T("유효하지 않는 메트릭 값"));
        return;
    }

    // 싱글톤 인스턴스를 가져와 setBuffEntry() 호출
    if (routingTable.setBuffEntry(dst, mask, gate, flag, interfaceFlag, metricValue)) {
        routingTable.addEntry();
    }
    else {
        AfxMessageBox(_T("엔트리 추가 실패"));
    }

    // 부모 대화상자에 대한 기본 동작 수행
    CDialogEx::OnOK();
}

void RoutingDialog::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}
