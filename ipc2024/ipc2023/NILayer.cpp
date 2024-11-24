#include "stdafx.h"
#include "pch.h"
#include "NILayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CNILayer::CNILayer(char* pName, pcap_t* pAdapterObject, int iNumAdapter)
    : CBaseLayer(pName)
{
    m_AdapterObject = NULL;
    //memset(m_AdapterObjects, 0, sizeof(m_AdapterObjects));
    m_iNumAdapter = iNumAdapter;
    m_index = 0;
    m_thrdSwitch = FALSE;
    SetAdapterList();
}

CNILayer::~CNILayer()
{
    StopPacketDriver();
}

//순서 2번
/*
시스템에 연결된 모든 네트워크 어댑터 리스트를 설정합니다.
이 함수는 pcap_findalldevs() 함수를 사용해 시스템의 네트워크 어댑터 목록을 불러와
m_pAdapterList에 저장합니다. 각 어댑터는 사용자가 나중에 선택할 수 있으며,
목록을 초기화하여 사용 가능한 모든 어댑터를 배열에 저장합니다.
*/

void CNILayer::SetAdapterList() // Retrieve and set the device list
{
    /*
    struct pcap_if_t {
        struct pcap_if *next;          // 다음 네트워크 인터페이스를 가리키는 포인터
        char *name;                    // 네트워크 인터페이스 이름 (예: "eth0", "en0" 등)
        char *description;             // 인터페이스 설명 (예: "Intel(R) Ethernet Connection")
        struct pcap_addr *addresses;   // 인터페이스의 주소 목록을 가리키는 포인터
        bpf_u_int32 flags;             // 인터페이스 플래그 (예: PCAP_IF_LOOPBACK 등)
    };
    */
    pcap_if_t* AdtList;
    pcap_if_t* temp;

    char errbuf[PCAP_ERRBUF_SIZE];

    int devCount = 0;

    if (pcap_findalldevs(&AdtList, errbuf) == -1) {
        /*
        pcap_findalldevs: 사용 가능한 네트워크 장치들의 이름을 LinkedList의 형태로 반환하는 함수
            # parameter
            - pcap_if_t **: 사용 가능한 네트워크 장치들이 저장될 LinkedList
            - char * : 에러 버퍼
            # return value
            - 0 : 정상적으로 함수가 수행될 경우
            - -1 : 오류가 발생할 경우 / 에러 버퍼에 에러 메시지 저장
        */
        AfxMessageBox(_T("Not exist NICard"));
        return;
    }

    temp = AdtList;

    while (true) {
        if (!temp) break;
        m_pAdapterList[devCount] = temp;
        temp = temp->next;
        ++devCount;
    }

    m_iNumAdapter = devCount; // Store the number of adapters
}

//순서 3번
/*
대화상자 콤보박스에서 장치를 선택하면 CNILayer의 멤버변수인 m_iNumAdapter를 설정합니다.
*/

void CNILayer::SetAdapterIndex(int index)
{
    m_index = index;
}

pcap_if_t* CNILayer::GetAdapterObject(int index)
{
    if (index < 0 || index >= m_iNumAdapter) {
        return nullptr;  // 인덱스가 범위를 벗어나면 nullptr 반환
    }
    return m_pAdapterList[index];
}

//순서4번
/*네트워크 어댑터의 MAC 주소를 가져오는 함수
이 함수는 지정된 어댑터 이름을 사용하여 해당 어댑터의 MAC 주소를 가져옵니다.
PacketOpenAdapter()와 PacketRequest() 함수를 통해 MAC 주소를 얻어온 후,
그 값을 반환합니다.
*/

CString CNILayer::GetNICardAddress(char* adapter_name)
{

    PPACKET_OID_DATA OidData; //MAC주소 저장
    LPADAPTER Adapter = 0; //네트워크 어댑터를 가리키는 포인터


    OidData = (PPACKET_OID_DATA)malloc(6 + sizeof(PACKET_OID_DATA));
    OidData->Oid = OID_802_3_CURRENT_ADDRESS; //주소를 불러오는 표준상수 *값(0x01010102)
    OidData->Length = 6;

    Adapter = PacketOpenAdapter(adapter_name); // 어댑터 열고  어댑터 핸들을 통해 어댑터 접근 권한

    PacketRequest(Adapter, FALSE, OidData); //어댑터이름, (F)읽기요청 (T)변경요청, 이 필드에 네트워크 어댑터의 MAC 주소가 저장

    CString NICardAddress;

    NICardAddress.Format("%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
        (OidData->Data)[0],
        (OidData->Data)[1],
        (OidData->Data)[2],
        (OidData->Data)[3],
        (OidData->Data)[4],
        (OidData->Data)[5]);

    PacketCloseAdapter(Adapter);
    free(OidData);
    return NICardAddress;

}

//순서 5번
/*
네트워크 패킷을 전송하는 함수입니다.
이 함수는 주어진 데이터를 네트워크로 전송합니다.
pcap_sendpacket() 함수를 호출하여 패킷을 전송하며,
전송이 성공했는지 여부에 따라 TRUE 또는 FALSE를 반환합니다.
*/

BOOL CNILayer::Send(unsigned char* payload_data, int payload_data_len)
{
    if (pcap_sendpacket(m_AdapterObject, payload_data, payload_data_len)) {
        char *err = pcap_geterr(m_AdapterObject);
        AfxMessageBox(_T("Packet Send Failed"));
        AfxMessageBox(_T(err));
        return FALSE;
    }
    return TRUE;
}
//순서 6번
/*
패킷을 수신하여 상위 레이어로 전달하는 함수입니다.
이 함수는 수신된 패킷을 상위 레이어로 전달합니다.
 mp_aUpperLayer[0]->Receive()를 호출하여 패킷 데이터를 상위 레이어로 전달하는 기능을 수행합니다.
 */
BOOL CNILayer::Receive(unsigned char* payload_data)
{
    BOOL bSuccess = FALSE;
    bSuccess = mp_aUpperLayer[0]->Receive(payload_data);
    return bSuccess;
}

//순서 7번
/*
멀티스레드에서 패킷을 비동기적으로 수신하는 함수입니다
이 함수는 pcap_next_ex()를 사용해 패킷을 비동기적으로 수신하며,
수신된 패킷을 Receive()를 통해 상위 레이어로 전달합니다.
멀티스레드를 사용해 패킷 수신 작업이 비동기적으로 처리되므로,
네트워크에서 패킷이 도착할 때마다 처리할 수 있습니다.
*/


UINT CNILayer::ReadingThread(LPVOID pParam)
{
    CNILayer* pNI = (CNILayer*)pParam;
    struct pcap_pkthdr* header;
    const u_char* pkt_data;
    int result;

    for (; pNI->m_thrdSwitch;)
    {
        for (result = pcap_next_ex(pNI->m_AdapterObject, &header, &pkt_data)
            ; result >= 0; result = pcap_next_ex(pNI->m_AdapterObject, &header, &pkt_data))
            /*
            pcap_next_ex: 패킷을 읽어오는 함수
            # parameter
            - pcap_t *p : 네트워크 장치 핸들러
            - struct pcap_pkthdr **pkt_header : 캡처한 패킷의 헤더에 대한 포인터
                - 함수를 거치며 저장됨.
            - const u_char **pkt_data : 캡처한 패킷의 데이터
                - 함수를 거치며 저장됨
            # return value
            - 1 : 성공
            - 0 : 타임아웃 시간 동안 패킷이 도착하지 않음
            - -1 : 패킷 읽는 도중 오류 발생
            - -2 : 패킷 캡처가 EOF에 도달
            */
        {
            if (result == 0)
            {
                continue;
            }
            pNI->Receive((u_char*)pkt_data);
        }

        if (result < 0)
        {
            AfxMessageBox(_T("Packet Read Error"));
            break;
        }
    }

    return 0;
}

//순서 1번
/*
네트워크 패킷을 수신하기 위한 드라이버를 시작하고 네트워크 어댑터로부터 패킷을 수신하기 위한 준비를 합니다.
대화상자에서 설정 버튼을 누르면 활성화가 됩니다.
CNILayer의 멤버변수 m_iNumAdapter을 활용하여 선택한 어댑터에 대한 핸들러를 얻습니다.
이를 멤버변수에 저장합니다.
*/

BOOL CNILayer::PacketStartDriver(int index)
{
    char errbuf[PCAP_ERRBUF_SIZE];

    m_AdapterObject = pcap_open_live(m_pAdapterList[m_index]->name, 65536, 1, 2000, errbuf);
    /*
    pcap_open_live: 인자로 주어진 네트워크 장치에 대한 라이브 패킷 캡처를 시작하는 함수
        # paremeter
        - const char *device: 캡처할 네트워크 장치 이름
        - int snaplen: 캡처할 최대 바이트 수
        - int promisc: 프로미스 큐 모드 설정
            - 1: 모든 패킷 캡처
            - 0: 해당 장치의 패킷만 캡처
        - int to_ms: 패킷을 캡처할 때 대기할 최대 시간
            - 설정 시간 동안 패킷이 도착하지 않으면 timeout
        - char *errbuf: 오류 메시지를 저장할 버퍼
        # return value
        - pcap_t*: 라이브 패킷 캡처가 성공적으로 열리면 해당 장치에 대한 핸들러를 반환
        - NULL: 오류가 발생하면 NULL 리턴 / errbuf에 에러 메시지 저장.
    */

    m_thrdSwitch = TRUE; //패킷 수신 스레드 활성화
    AfxBeginThread(ReadingThread, this); //패킷을 수신하는 스레드 시작
    return TRUE;
}

//BOOL CNILayer::PacketStartDriver(int index)
//{
//    if (index < 0 || index >= m_iNumAdapter) {
//        AfxMessageBox(_T("Invalid adapter index."));
//        return FALSE;
//    }
//
//    char errbuf[PCAP_ERRBUF_SIZE] = { 0 };
//    pcap_t* adapterObject = pcap_open_live(m_pAdapterList[index]->name, 65536, 1, 2000, errbuf);
//    if (!adapterObject) {
//        CString err = CString(errbuf);
//        AfxMessageBox(_T("Failed to open adapter: ") + err);
//        return FALSE;
//    }
//
//    m_AdapterObjects[index] = adapterObject; // 각 인덱스별 어댑터 객체 저장
//    m_thrdSwitch = TRUE; // 수신 스레드 활성화
//    AfxBeginThread(ReadingThread, this, THREAD_PRIORITY_NORMAL, 0, 0, (LPVOID)index); // 수신 스레드 시작
//    return TRUE;
//}

//순서 7번
//BOOL CNILayer::StopPacketDriver()
//{
//    m_thrdSwitch = FALSE; // 모든 스레드 종료
//
//    for (int i = 0; i < m_iNumAdapter; ++i) {
//        if (m_AdapterObjects[i]) {
//            pcap_close(m_AdapterObjects[i]); // 어댑터 닫기
//            m_AdapterObjects[i] = NULL;
//        }
//    }
//    return TRUE;
//}

BOOL CNILayer::StopPacketDriver()
{
    m_thrdSwitch = FALSE; // 스레드 종료
    if (m_AdapterObject) {
        pcap_close(m_AdapterObject); // 어댑터 닫기
        m_AdapterObject = NULL;
    }
    return TRUE;
}

