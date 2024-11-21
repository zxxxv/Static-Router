#include "pch.h"
#include "ipc2023.h"
#include "afxdialogex.h"
#include "ProxyDialog.h"
#include "ipc2023Dlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// ProxyDialog 대화 상자

IMPLEMENT_DYNAMIC(ProxyDialog, CDialogEx)

ProxyDialog::ProxyDialog(CWnd* pParent /*=nullptr*/, pcap_if_t** adapterList /*=nullptr*/)
    : CDialogEx(IDD_DIALOG1, pParent), m_index(0)
{
    m_pAdapterList = adapterList;

}

BOOL ProxyDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    UpdateData(TRUE); // OnInitDialog에서는 필요하지 않음
    CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBOP);
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


ProxyDialog::~ProxyDialog()
{
}

void ProxyDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    //  DDX_Control(pDX, IDC_COMBOP, m_combop); // IDC_COMBOP과 m_combop 연결
    DDX_Control(pDX, IDC_COMBOP, m_combop);
    DDX_Control(pDX, IDC_IPADDRESSP, m_ipaddressp);
    DDX_Control(pDX, IDC_EDITP, m_editp);
}

BEGIN_MESSAGE_MAP(ProxyDialog, CDialogEx)
    ON_BN_CLICKED(IDOK, &ProxyDialog::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &ProxyDialog::OnBnClickedCancel)
END_MESSAGE_MAP()

void ProxyDialog::OnBnClickedOk()
{
    
    BYTE ipBytes[4];
    m_ipaddressp.GetAddress(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]);
    m_strIPAddress.Format(_T("%d.%d.%d.%d"), ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]);

    m_editp.GetWindowText(m_strMACAddress);
    m_combop.GetLBText(m_combop.GetCurSel(), m_strDeviceName); // 선택된 장치 이름 가져오기

    CDialogEx::OnOK();
}

void ProxyDialog::OnBnClickedCancel()
{
    CDialogEx::OnCancel();
}