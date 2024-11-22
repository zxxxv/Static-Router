// ipc2023Dlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ipc2023.h"
#include "ipc2023Dlg.h"
#include "afxdialogex.h"
#include "ProxyDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX); // 
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cipc2023Dlg 대화 상자

Cipc2023Dlg::Cipc2023Dlg(CWnd* pParent /*=nullptr*/) // Cipc2023Dlg의 생성자 구현
	: CDialogEx(IDD_IPC2023_DIALOG, pParent)
	, CBaseLayer("ChatDlg") // CBaseLayer의 생성자를 호출하여 ChatDlg라는 레이어를 생성한다.
	, m_bSendReady(FALSE)
	, m_index(0)
{

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//Protocol Layer Setting
	m_LayerMgr.AddLayer(new CEthernetLayer("Ethernet"));
	m_LayerMgr.AddLayer(new CNILayer("NI"));
	m_LayerMgr.AddLayer(new CIPLayer("IP"));
	m_LayerMgr.AddLayer(this);

	// 레이어를 연결한다. (레이어 생성)
	m_LayerMgr.ConnectLayers("NI ( *Ethernet ( *IP ( *ChatDlg ) ) )");

	m_Eth = (CEthernetLayer*)m_LayerMgr.GetLayer("Ethernet");
	m_NI = (CNILayer*)m_LayerMgr.GetLayer("NI");
	m_IP = (CIPLayer*)m_LayerMgr.GetLayer("IP");
}

void Cipc2023Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_COMBO, m_comboBox);
	DDX_Control(pDX, IDC_LIST_CTR, m_ListCtrl);
	DDX_Control(pDX, IDC_LIST_CTR_P, m_ListCtrlP);
	DDX_Control(pDX, IDC_LIST_CTR_RTABLE, m_ListCtrlR);
	DDX_Text(pDX, IDC_EDIT_MAC1, m_iMacSrc);
	DDX_Text(pDX, IDC_EDIT_MAC2, m_oMacSrc);
	DDX_Control(pDX, IDC_COMBO_MAC1, m_comboBox1);
	DDX_Control(pDX, IDC_COMBO_MAC2, m_comboBox2);
}

BEGIN_MESSAGE_MAP(Cipc2023Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_CBN_SELCHANGE(IDC_COMBO, &Cipc2023Dlg::OnCbnSelchangeCombo)
	ON_BN_CLICKED(IDC_ARP_TABLE, &Cipc2023Dlg::OnBnClickedArpTable)
	ON_BN_CLICKED(IDC_PROXY_ADD, &Cipc2023Dlg::OnBnClickedProxyAdd)
	ON_BN_CLICKED(IDC_PROXY_DELETE, &Cipc2023Dlg::OnBnClickedProxyDelete)
	ON_BN_CLICKED(IDC_PROXY_TABLE, &Cipc2023Dlg::OnBnClickedProxyTable)
	ON_BN_CLICKED(IDC_BUTTON_END, &Cipc2023Dlg::OnBnClickedButtonEnd)
	ON_BN_CLICKED(IDC_BUTTON_START, &Cipc2023Dlg::OnBnClickedButtonStart)
	ON_CBN_SELCHANGE(IDC_COMBO_MAC1, &Cipc2023Dlg::OnCbnSelchangeComboMac)
	ON_CBN_SELCHANGE(IDC_COMBO_MAC2, &Cipc2023Dlg::OnCbnSelchangeComboMac2)
END_MESSAGE_MAP()


// Cipc2023Dlg 메시지 처리기

BOOL Cipc2023Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	UpdateData(FALSE);
	//SetRegstryMessage();
	SetDlgState(IPC_INITIALIZING);
	SetDlgState(IPC_COMBO_SET);

	// ListCtrl에 제목 추가
	
	// ARP Table
	CRect rtA;
	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_ListCtrl.InsertColumn(0, _T("IP Address"), LVCFMT_LEFT, rtA.Width()+183);
	m_ListCtrl.InsertColumn(1, _T("Ethernet Address"), LVCFMT_LEFT, rtA.Width()+220);
	m_ListCtrl.InsertColumn(2, _T("Status"), LVCFMT_LEFT, rtA.Width()+150);

	// PROXY Table
	CRect rtP;
	m_ListCtrlP.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_ListCtrlP.InsertColumn(0, _T("Device"), LVCFMT_LEFT, rtP.Width() +220);
	m_ListCtrlP.InsertColumn(1, _T("IP Address"), LVCFMT_LEFT, rtP.Width() + 183);
	m_ListCtrlP.InsertColumn(2, _T("Ethernet Address"), LVCFMT_LEFT, rtP.Width() + 200);

	// ROUTING Table
	CRect rt;
	m_ListCtrlR.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_ListCtrlR.InsertColumn(0, _T("Destination"), LVCFMT_LEFT, rt.Width() + 180);
	m_ListCtrlR.InsertColumn(1, _T("NetMask"), LVCFMT_LEFT, rt.Width() + 180);
	m_ListCtrlR.InsertColumn(2, _T("GateWay"), LVCFMT_LEFT, rt.Width() + 180);
	m_ListCtrlR.InsertColumn(3, _T("Flag"), LVCFMT_LEFT, rt.Width() + 90);
	m_ListCtrlR.InsertColumn(4, _T("Interface"), LVCFMT_LEFT, rt.Width() + 180);
	m_ListCtrlR.InsertColumn(5, _T("Metric"), LVCFMT_LEFT, rt.Width() + 110);

	//GetDlgItem(IDC_BUTTON_IP_SEND)->EnableWindow(FALSE);

	//GetDlgItem(IDC_BUTTON_GARP_SEND)->EnableWindow(FALSE);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void Cipc2023Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void Cipc2023Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR Cipc2023Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL Cipc2023Dlg::Receive(unsigned char* ppayload)
{
	unsigned char* mac = ppayload;
	unsigned char* ip = ppayload + 6;

	// IP, MAC, Status를 CString으로 변환
	CString strIP, strMAC, strStatus;

	// IP 변환
	strIP.Format(_T("%d.%d.%d.%d"), ip[0], ip[1], ip[2], ip[3]);

	// MAC 변환
	strMAC.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"),
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	// Status 변환
	strStatus.Format(_T("%s"), "complete");

	//CString message;
	//message.Format(_T("변경 IP: %s, MAC: %s, Status: %s"), strIP, strMAC, strStatus);
	//AfxMessageBox(message);

	// 리스트 컨트롤 업데이트
	UpdateListCtrlItem(strIP, strMAC, strStatus);

	return TRUE;
}

BOOL Cipc2023Dlg::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			//if (::GetDlgCtrlID(::GetFocus()) == IDC_EDIT3)
				//OnBnClickedButtonSend();
			return FALSE;
		case VK_ESCAPE: return FALSE;
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void Cipc2023Dlg::SetDlgState(int state)
{
	UpdateData(TRUE);

	CComboBox* pComboBox1 = (CComboBox*)GetDlgItem(IDC_COMBO_MAC1);

	switch (state)
	{
	case IPC_INITIALIZING:
		//pSendButton->EnableWindow(FALSE);
		//pMsgEdit->EnableWindow(FALSE);
		//m_ListChat.EnableWindow(FALSE);
		break;
	case IPC_READYTOSEND:
		//pSendButton->EnableWindow(TRUE);
		//pMsgEdit->EnableWindow(TRUE);
		//m_ListChat.EnableWindow(TRUE);
		break;
	case IPC_WAITFORACK:	break;
	case IPC_ERROR:		break;
	case IPC_COMBO_SET:
		for (int i = 0; i < NI_COUNT_NIC; ++i) {
			pcap_if_t* tempAdater = m_NI->GetAdapterObject(i);
			if (!tempAdater) continue;
			pComboBox1->AddString(tempAdater->description);
			pComboBox1->SetCurSel(0);
		}
	}

	UpdateData(FALSE);
}


void Cipc2023Dlg::EndofProcess()
{
	m_LayerMgr.DeAllocLayer();
}

void Cipc2023Dlg::Str2UCHAR(CString& src, UCHAR* dst)
{
	sscanf_s(src, "%02x:%02x:%02x:%02x:%02x:%02x",
		&dst[0], &dst[1], &dst[2],
		&dst[3], &dst[4], &dst[5]);
}

void Cipc2023Dlg::UCHAR2Str(UCHAR* src, CString& dst)
{
	dst.Format(_T("%02x:%02x:%02x:%02x:%02x:%02x"),
		src[0], src[1], src[2],
		src[3], src[4], src[5]);
}

void Cipc2023Dlg::OnCbnSelchangeComboMac()
{
	// 내부 어댑터 선택
	UpdateData(TRUE);
	m_index = m_comboBox1.GetCurSel();
	m_NI->SetAdapterIndex(m_index);
	pcap_if_t* selectedAdapter = m_NI->GetAdapterObject(m_index);
	CString selectedAdapterAdress = m_NI->GetNICardAddress(selectedAdapter->name);
	m_iMacSrc = selectedAdapterAdress;
	CEdit* pSrcEdit = (CEdit*)GetDlgItem(IDC_EDIT_MAC1);
	pSrcEdit->SetWindowTextA(m_iMacSrc);
	UpdateData(FALSE);
}


void Cipc2023Dlg::OnCbnSelchangeComboMac2()
{
	// 외부 어댑터 선택

}

void Cipc2023Dlg::OnBnClickedButtonDelete() // 삭제 버튼
{
	POSITION pos;
	pos = m_ListCtrl.GetFirstSelectedItemPosition();

	int idx = m_ListCtrl.GetNextSelectedItem(pos);

	if (idx != -1)
	{		
		CString strValue = m_ListCtrl.GetItemText(idx, 0);
		unsigned char value[4];
		int ip1, ip2, ip3, ip4;
		_stscanf_s(strValue, _T("%d.%d.%d.%d"), &ip1, &ip2, &ip3, &ip4);
		value[0] = static_cast<unsigned char>(ip1);
		value[1] = static_cast<unsigned char>(ip2);
		value[2] = static_cast<unsigned char>(ip3);
		value[3] = static_cast<unsigned char>(ip4);

		m_ListCtrl.DeleteItem(idx);
		// ARP 캐시 테이블에 있는 엔트리 제거하기
		//m_ARP->onEntryTimeout(value);
		m_IP->removeEntry(value);
		
		//m_ARP->printCache();
	}
}

void Cipc2023Dlg::UpdateListCtrlItem(const CString& ip, const CString& mac, const CString& status) // ListCtrl 수정
{
	int itemCount = m_ListCtrl.GetItemCount();
	bool itemFound = false;

	for (int i = 0; i < itemCount; ++i)
	{
		CString column1Value = m_ListCtrl.GetItemText(i, 0);

		if (column1Value == ip)
		{
			m_ListCtrl.SetItemText(i, 1, mac);
			m_ListCtrl.SetItemText(i, 2, status);
			itemFound = true;
			break;
		}
	}

	// 동일한 IP를 찾지 못한 경우에만 새 항목 추가
	if (!itemFound)
	{
		int num = m_ListCtrl.GetItemCount();
		m_ListCtrl.InsertItem(num, ip);
		m_ListCtrl.SetItem(num, 1, LVIF_TEXT, mac, 0, 0, 0, 0);
		m_ListCtrl.SetItem(num, 2, LVIF_TEXT, status, 0, 0, 0, 0);
	}
}

void Cipc2023Dlg::OnBnClickedArpTable()
{
	m_IP->printCache();
}

void Cipc2023Dlg::TimeoutEntryDelete(const unsigned char* ip)
{
	int Index = 0;
	int itemCount = m_ListCtrl.GetItemCount(); // 전체 항목 개수
	CString tarIP;
	tarIP.Format(_T("%d.%d.%d.%d"), ip[0], ip[1], ip[2], ip[3]);

	for (int i = 0; i < itemCount; ++i) {
		CString itemText = m_ListCtrl.GetItemText(i, 0); // 특정 열의 텍스트 가져오기
		if (itemText.Find(tarIP) != -1) { // searchText가 포함된 항목을 찾으면
			Index = i; // 해당 인덱스 반환
			break;
		}
	}
	m_ListCtrl.DeleteItem(Index);
}

void Cipc2023Dlg::OnBnClickedProxyAdd() // 프록시 테이블 추가
{
	ProxyDialog dlg(nullptr, m_NI->m_pAdapterList);
	if (dlg.DoModal() == IDOK)
	{
		CString ipAddress = dlg.m_strIPAddress;
		CString macAddress = dlg.m_strMACAddress;
		CString deviceName = dlg.m_strDeviceName;

		int nIndex = m_ListCtrlP.InsertItem(m_ListCtrlP.GetItemCount(), deviceName);
		m_ListCtrlP.SetItemText(nIndex, 1, ipAddress);
		m_ListCtrlP.SetItemText(nIndex, 2, macAddress);

		unsigned char ipBytes[4] = { 0 };
		_stscanf_s(ipAddress, _T("%hhu.%hhu.%hhu.%hhu"), &ipBytes[0], &ipBytes[1], &ipBytes[2], &ipBytes[3]);

		unsigned char macBytes[6] = { 0 };
		_stscanf_s(macAddress, _T("%hhx:%hhx:%hhx:%hhx:%hhx:%hhx"), &macBytes[0], &macBytes[1], &macBytes[2], &macBytes[3], &macBytes[4], &macBytes[5]);

		proxyTable.AddEntry(deviceName, ipBytes, macBytes);
	}
}


void Cipc2023Dlg::OnBnClickedProxyDelete() // 프록시 테이블 삭제
{
	POSITION posP;
	posP = m_ListCtrlP.GetFirstSelectedItemPosition();
	int idx = m_ListCtrlP.GetNextSelectedItem(posP);

	if (idx != -1) {
		// 선택된 항목의 IP 주소 문자열 가져오기
		CString strValue = m_ListCtrlP.GetItemText(idx, 1);
		unsigned char value[4];
		int ip1, ip2, ip3, ip4;
		_stscanf_s(strValue, _T("%d.%d.%d.%d"), &ip1, &ip2, &ip3, &ip4);
		value[0] = static_cast<unsigned char>(ip1);
		value[1] = static_cast<unsigned char>(ip2);
		value[2] = static_cast<unsigned char>(ip3);
		value[3] = static_cast<unsigned char>(ip4);

		// 리스트 컨트롤에서 항목 삭제
		m_ListCtrlP.DeleteItem(idx);

		// 프록시 테이블에 있는 엔트리 제거하기
		if (!proxyTable.RemoveEntryByIP(value)) AfxMessageBox(_T("삭제 실패"));
	}
}

void Cipc2023Dlg::OnBnClickedProxyTable()
{	
	proxyTable.DisplayAllEntries();
}

void Cipc2023Dlg::OnBnClickedButtonEnd()
{
	// receive 쓰레드 종료
}

void Cipc2023Dlg::OnBnClickedButtonStart()
{
	// GARP Send
	
	// 내부 네트워크 어댑터 receive 쓰레드 시작
	m_NI->PacketStartDriver();
}

// CtrlList 업데이트 수정하기 
// 하나씩 추가 삭제 하는식으로 하지말고 Table에 직접 적용시키고 Table 전체 업데이트 시키기
// 외부 네트워크와 연결된 어댑터2 쓰레드로 동작 시키기