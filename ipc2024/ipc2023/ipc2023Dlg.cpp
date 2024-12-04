// ipc2023Dlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ipc2023.h"
#include "ipc2023Dlg.h"
#include "afxdialogex.h"
#include "ProxyDialog.h"
#include "RoutingDialog.h"

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
	, m_routerReady(FALSE)
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
	DDX_Control(pDX, IDC_IPADDRESS1, m_ip1);
	DDX_Control(pDX, IDC_IPADDRESS2, m_ip2);
}

BEGIN_MESSAGE_MAP(Cipc2023Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PROXY_ADD, &Cipc2023Dlg::OnBnClickedProxyAdd)
	ON_BN_CLICKED(IDC_PROXY_DELETE, &Cipc2023Dlg::OnBnClickedProxyDelete)
	//ON_BN_CLICKED(IDC_PROXY_TABLE, &Cipc2023Dlg::OnBnClickedProxyTable)
	ON_BN_CLICKED(IDC_BUTTON_END, &Cipc2023Dlg::OnBnClickedButtonEnd)
	ON_BN_CLICKED(IDC_BUTTON_START, &Cipc2023Dlg::OnBnClickedButtonStart)
	ON_CBN_SELCHANGE(IDC_COMBO_MAC1, &Cipc2023Dlg::OnCbnSelchangeComboMac)
	ON_CBN_SELCHANGE(IDC_COMBO_MAC2, &Cipc2023Dlg::OnCbnSelchangeComboMac2)
	ON_BN_CLICKED(IDC_BUTTON_RADD, &Cipc2023Dlg::OnBnClickedButtonRadd)
	ON_BN_CLICKED(IDC_BUTTON_RDELETE, &Cipc2023Dlg::OnBnClickedButtonRdelete)
	ON_BN_CLICKED(IDC_BUTTON_ARP_DELETE, &Cipc2023Dlg::OnBnClickedButtonArpDelete)
	ON_BN_CLICKED(IDC_BUTTON1, &Cipc2023Dlg::OnBnClickedButton1)
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

	SetDlgState(IPC_ROUTEREND);
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
	m_ListCtrlR.InsertColumn(3, _T("Flag"), LVCFMT_LEFT, rt.Width() + 50);
	m_ListCtrlR.InsertColumn(4, _T("Interface"), LVCFMT_LEFT, rt.Width() + 190);
	m_ListCtrlR.InsertColumn(5, _T("Metric"), LVCFMT_LEFT, rt.Width() + 70);

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

//BOOL Cipc2023Dlg::Receive(unsigned char* ppayload, int io)
//{
//	return TRUE;
//}

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
	CComboBox* pComboBox2 = (CComboBox*)GetDlgItem(IDC_COMBO_MAC2);
	CButton* pStartButton = (CButton*)GetDlgItem(IDC_BUTTON_START);
	CButton* pEndButton = (CButton*)GetDlgItem(IDC_BUTTON_END);

	switch (state)
	{
	case IPC_ROUTERSTART:
		/*pStartButton->EnableWindow(FALSE);
		pEndButton->EnableWindow(TRUE);*/
		if (pStartButton) pStartButton->EnableWindow(FALSE);
		if (pEndButton) pEndButton->EnableWindow(TRUE);
		break;
	case IPC_ROUTEREND:
		//pStartButton->EnableWindow(TRUE);
		//pEndButton->EnableWindow(FALSE);
		if (pStartButton) pStartButton->EnableWindow(TRUE);
		if (pEndButton) pEndButton->EnableWindow(FALSE);
		break;
	case IPC_WAITFORACK:	break;
	case IPC_ERROR:		break;
	case IPC_COMBO_SET:
		for (int i = 0; i < NI_COUNT_NIC; ++i) {
			pcap_if_t* adt = m_NI->m_pAdapterList[i];
			//if (!adt) continue;
			if (adt == nullptr) break;
			pComboBox1->AddString(CString(adt->description));
			pComboBox1->SetCurSel(0);
			pComboBox2->AddString(CString(adt->description));
			pComboBox2->SetCurSel(0);
		}
	}

	UpdateData(FALSE);
}


void Cipc2023Dlg::EndofProcess()
{
	m_LayerMgr.DeAllocLayer();
}

void Cipc2023Dlg::OnCbnSelchangeComboMac() // 0
{
	// 내부 어댑터 선택
	UpdateData(TRUE);
	pcap_if_t* temp;
	m_index = m_comboBox1.GetCurSel();
	m_NI->SetAdapterIndex(m_index);
	temp = m_NI->m_pAdapterList[m_index];
	m_NI->GetAdapterObject(0).initAdapter(temp, 0);
	CString selectedAdapterAdress = m_NI->GetAdapterObject(0).getMacAddr();
	m_iMacSrc = selectedAdapterAdress;
	CEdit* pSrcEdit = (CEdit*)GetDlgItem(IDC_EDIT_MAC1);
	pSrcEdit->SetWindowTextA(m_iMacSrc);
	UpdateData(FALSE);
}

void Cipc2023Dlg::OnCbnSelchangeComboMac2() // 1
{
	// 외부 어댑터 선택
	UpdateData(TRUE);
	pcap_if_t* temp;
	m_index = m_comboBox2.GetCurSel();
	//m_NI->SetAdapterIndex(m_index);
	temp = m_NI->m_pAdapterList[m_index];
	m_NI->GetAdapterObject(1).initAdapter(temp, 1);
	CString selectedAdapterAdress = m_NI->GetAdapterObject(1).getMacAddr();
	m_oMacSrc = selectedAdapterAdress;
	CEdit* pSrcEdit = (CEdit*)GetDlgItem(IDC_EDIT_MAC2);
	pSrcEdit->SetWindowTextA(m_oMacSrc);
	UpdateData(FALSE);
}

void Cipc2023Dlg::UpdateRoutingTable() // 라우팅 테이블 출력
{
	// 리스트 컨트롤 초기화
	m_ListCtrlR.DeleteAllItems();

	// 라우팅 테이블에서 모든 엔트리를 가져와 리스트 컨트롤에 추가
	std::list<RoutingEntry::Fields> routingEntries = routingTable.getAllEntries();
	int index = 0;
	for (const auto& entry : routingEntries) {
		CString strDestination, strNetmask, strGateway, strFlag, strInterface, strMetric;

		strDestination.Format(_T("%d.%d.%d.%d"), entry.m_destination[0], entry.m_destination[1], entry.m_destination[2], entry.m_destination[3]);
		strNetmask.Format(_T("%d.%d.%d.%d"), entry.m_subnetMask[0], entry.m_subnetMask[1], entry.m_subnetMask[2], entry.m_subnetMask[3]);
		strGateway.Format(_T("%d.%d.%d.%d"), entry.m_gateway[0], entry.m_gateway[1], entry.m_gateway[2], entry.m_gateway[3]);
		strFlag = GetFlagString(entry.m_flag);
		strInterface = CString(m_NI->m_pAdapterList[entry.m_interfaceFlag]->description);
		strMetric.Format(_T("%d"), entry.m_metric);

		int nIndex = m_ListCtrlR.InsertItem(index++, strDestination);
		m_ListCtrlR.SetItemText(nIndex, 1, strNetmask);
		m_ListCtrlR.SetItemText(nIndex, 2, strGateway);
		m_ListCtrlR.SetItemText(nIndex, 3, strFlag);
		m_ListCtrlR.SetItemText(nIndex, 4, strInterface);
		m_ListCtrlR.SetItemText(nIndex, 5, strMetric);
	}
}

void Cipc2023Dlg::UpdateARPTable() // ARP 테이블 출력
{
	m_ListCtrl.DeleteAllItems(); // 리스트 컨트롤 초기화
	CString arpentries = m_IP->printCache(); // ARP 엔트리 가져오기
	
	int nIndex = 0;
	int start = 0;
	CString line = arpentries.Tokenize(_T("\r\n"), start);

	while (!line.IsEmpty()) {
		CString ip, mac, state;

		//IP, MAC, State가 시작하는 index번호를 추출
		int ipStart = line.Find(_T("IP: ")) + 4;
		int macStart = line.Find(_T("MAC: ")) + 5;
		int stateStart = line.Find(_T("State: ")) + 7;

		//IP, MAC, State의 실질적 데이터 추출, line의 형식이 IP: %s, MAC: %s, State: %s
		ip = line.Mid(ipStart, macStart - ipStart - 7).Trim();
		mac = line.Mid(macStart, stateStart - macStart - 9).Trim();
		state = line.Mid(stateStart).Trim();

		int itemIndex = m_ListCtrl.InsertItem(nIndex++, ip);  //첫 번째 열: IP
		m_ListCtrl.SetItemText(itemIndex, 1, mac);  //두 번째 열: MAC
		m_ListCtrl.SetItemText(itemIndex, 2, state);  // 세 번째 열: state


		// 다음 라인으로 이동
		line = arpentries.Tokenize(_T("\r\n"), start);

	}
}

CString Cipc2023Dlg::GetFlagString(e_flag flag)
{
	switch (flag) {
	case e_flag::none:
		return _T("None");
	case e_flag::up:
		return _T("U");
	case e_flag::gateway:
		return _T("UG");
	default:
		return _T("");
	}
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

void Cipc2023Dlg::OnBnClickedButtonEnd()
{
	// receive 쓰레드 종료
	m_NI->GetAdapterObject(INNER).StopPacketDriver();
	m_NI->GetAdapterObject(OUTER).StopPacketDriver();
	m_routerReady = FALSE;
	SetDlgState(IPC_ROUTEREND);
}

void Cipc2023Dlg::OnBnClickedButtonStart()
{
	// GARP Send
	//INTERFACE_CARD interfaces[2];
	
	// 내부 인터페이스 IP 설정
	unsigned char ip1[4] = {0};
	m_ip1.GetAddress(ip1[0], ip1[1], ip1[2], ip1[3]); // 내부 IP 입력
	memcpy(interfaces[0].ipAddr, ip1, 4);

	// 외부 인터페이스 IP 설정
	unsigned char ip2[4] = {0};
	m_ip2.GetAddress(ip2[0], ip2[1], ip2[2], ip2[3]); // 외부 IP 입력
	memcpy(interfaces[1].ipAddr, ip2, 4);

	// MAC 주소 변환 및 설정
	Converter::macSTR2B(Converter::CS2STR(m_iMacSrc), interfaces[0].macAddr);
	Converter::macSTR2B(Converter::CS2STR(m_oMacSrc), interfaces[1].macAddr);


	m_IP->SetInterfaceInfo(interfaces[0].macAddr, interfaces[0].ipAddr, interfaces[1].macAddr, interfaces[1].ipAddr);
	m_Eth->SetInterfaceInfo(interfaces[0].macAddr, interfaces[1].macAddr);

	BOOL ready1 = FALSE;
	BOOL ready2 = FALSE;

	if (m_NI->GetAdapterObject(INNER).PacketStartDriver()) {
		if (m_IP->createGarpPacket(0)) {// GARP 패킷 전송
			ready1 = TRUE;
		}
	}
	if (m_NI->GetAdapterObject(OUTER).PacketStartDriver()) {
		if (m_IP->createGarpPacket(1)) {// GARP 패킷 전송
			ready2 = TRUE;
		}
	}
	if (ready1 && ready2) {
		m_routerReady = TRUE;
		SetDlgState(IPC_ROUTERSTART);
	}	
}

void Cipc2023Dlg::OnBnClickedButtonRadd() // Routing Entry 추가
{
	RoutingDialog dlg(nullptr, m_NI->m_pAdapterList);
	if (dlg.DoModal() == IDOK)
	{
		UpdateRoutingTable();
	}
}

void Cipc2023Dlg::OnBnClickedButtonRdelete() // Routing Entry 삭제
{
	POSITION posR;
	posR = m_ListCtrlR.GetFirstSelectedItemPosition();
	int idx = m_ListCtrlR.GetNextSelectedItem(posR);

	if (idx != -1) {
		// 라우팅 테이블에서 엔트리 삭제
		if (!routingTable.deleteEntry(idx)) {
			AfxMessageBox(_T("라우팅 엔트리 삭제 실패"));
		}
		// 라우팅 테이블 리스트 컨트롤 업데이트
		UpdateRoutingTable();
	}
}

void Cipc2023Dlg::OnBnClickedButtonArpDelete() // ARP Entry 삭제
{
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	int idx = m_ListCtrl.GetNextSelectedItem(pos);

	if (idx != -1) {
		// 리스트 컨트롤에서 선택된 IP 주소 가져오기
		CString ip = m_ListCtrl.GetItemText(idx, 0);

		unsigned char value[4];
		int ip1, ip2, ip3, ip4;
		_stscanf_s(ip, _T("%d.%d.%d.%d"), &ip1, &ip2, &ip3, &ip4);
		value[0] = static_cast<unsigned char>(ip1);
		value[1] = static_cast<unsigned char>(ip2);
		value[2] = static_cast<unsigned char>(ip3);
		value[3] = static_cast<unsigned char>(ip4);

		// removeEntry 호출
		if (!m_IP->removeEntry(value)) {
			AfxMessageBox(_T("APR 엔트리 삭제 실패"));
		}

		// 리스트 컨트롤 업데이트
		UpdateARPTable();
	}
}

void Cipc2023Dlg::OnBnClickedButton1()
{
	//m_IP->print();
	//unsigned char tip[4] = { 1,1,2,2 };
	//m_IP->createArpRequestPacket(tip, 1);

	
	//unsigned char dest_ip[4] = { 1,1,2,1 };
	//unsigned char b = 128;

	//unsigned char srcMac_ip[4] = {0,0,0,0};

	//if (m_IP->Routing(dest_ip)) {
	//	memcpy(srcMac_ip, m_IP->Routing(dest_ip), 4);
	//}
	//unsigned char srcMAC[6];

	//int ios = 0;

	//for (int i = 0; i < 2; i++) {
	//	if (memcmp(srcMac_ip, interfaces[i].ipAddr, 4) == 0) { //둘이 같으면
	//		memcpy(srcMAC, interfaces[i].macAddr, 6);
	//		ios = i;
	//		break;
	//	}
	//}

	unsigned char dest_ip[4] = { 1,1,2,1 };
	unsigned char* srcMac_ip = nullptr;
	//
	if (m_IP->Routing(dest_ip)) {
		srcMac_ip = m_IP->Routing(dest_ip);
	}
	unsigned char srcMAC[6];

	int ios = 0;

	for (int i = 0; i < 2; i++) {
		if (memcmp(srcMac_ip, interfaces[i].ipAddr, 4) == 0) { //둘이 같으면
			memcpy(srcMAC, interfaces[i].macAddr, 6);
			ios = i;
			break;
		}
	}

}
