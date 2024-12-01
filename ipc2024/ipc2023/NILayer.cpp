#include "stdafx.h"
#include "pch.h"
#include "NILayer.h"
#include "utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CNILayer::CNILayer(char* pName, int iNumAdapter)
    : CBaseLayer(pName)
{
    m_adapters.reserve(2);
    m_adapters.emplace_back(this); // 기본 생성자 호출
    m_adapters.emplace_back(this);
    m_iNumAdapter = iNumAdapter;
    m_index = 0;
    SetAdapterList();
}

CNILayer::~CNILayer()
{
    // StopPacketDriver();
}

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

/*
대화상자 콤보박스에서 장치를 선택하면 CNILayer의 멤버변수인 m_index를 설정합니다.
이는 콤보박스의 장치 선택과 PacketStartDriver가 분리되어 있기 때문.
PacketStartDriver에서 참조할 장치를 식별해야 하기 때문에 m_index의 설정이 필요하다.
*/

void CNILayer::SetAdapterIndex(int index)
{
    m_index = index;
}

Adapter& CNILayer::GetAdapterObject(int index)
{
    return m_adapters[index];
}

/*
패킷을 수신하여 상위 레이어로 전달하는 함수입니다.
이 함수는 수신된 패킷을 상위 레이어로 전달합니다.
 mp_aUpperLayer[0]->Receive()를 호출하여 패킷 데이터를 상위 레이어로 전달하는 기능을 수행합니다.

 Adapter에서 ReadingThread를 돌리면서 이 Recieve함수를 호출함.
 */

BOOL CNILayer::Receive(unsigned char* payload_data, int adtID)
{
    BOOL bSuccess = FALSE;
    bSuccess = mp_aUpperLayer[0]->Receive(payload_data, adtID);
    return bSuccess;
}
