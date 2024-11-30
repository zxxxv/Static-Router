#include "adapter.h"
#include "pch.h"

bool Adapter::StopPacketDriver()
{   
    m_thrdSwitch = FALSE; // 스레드 실행 플래그 비활성화
    if (m_pThread) {
        // 스레드가 종료될 때까지 대기
        WaitForSingleObject((HANDLE)m_pThread->m_hThread, INFINITE);
        m_pThread = nullptr; // 스레드 핸들 초기화
    }
    if (m_adapterHandler) {
        pcap_close(m_adapterHandler); // 어댑터 핸들 닫기
        m_adapterHandler = nullptr;
    }
    return TRUE;
}

std::string Adapter::GetNICardAddress(char* adapter_name)
{   
    PPACKET_OID_DATA OidData; //MAC주소 저장
    LPADAPTER Adapter = 0;

    OidData = (PPACKET_OID_DATA)malloc(6 + sizeof(PACKET_OID_DATA));
    OidData->Oid = OID_802_3_CURRENT_ADDRESS; //주소를 불러오는 표준상수 *값(0x01010102)
    OidData->Length = 6;

    Adapter = PacketOpenAdapter(adapter_name); // 어댑터 열고  어댑터 핸들을 통해 어댑터 접근 권한

    PacketRequest(Adapter, FALSE, OidData); //어댑터이름, (F)읽기요청 (T)변경요청, 이 필드에 네트워크 어댑터의 MAC 주소가 저장

    std::string NICardAddress = Converter::B2MAC(OidData->Data);

    PacketCloseAdapter(Adapter);
    free(OidData);
    return NICardAddress;
}

UINT Adapter::ReadingThread(LPVOID pParam) //
{
    Adapter* curAdapter = (Adapter*) pParam;
    int adtID = curAdapter->m_adtID;

    struct pcap_pkthdr* header;
    const u_char* pkt_data;
    int result;

    while (curAdapter->m_thrdSwitch) {
        for (result = pcap_next_ex(curAdapter->getHandler(), &header, &pkt_data)
            ; result >= 0; result = pcap_next_ex(curAdapter->getHandler(), &header, &pkt_data))
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
            curAdapter->m_pNILayer->Receive((u_char*)pkt_data, adtID);
        }

        if (result < 0)
        {
            AfxMessageBox(_T("Packet Read Error"));
            break;
        }
    }

    return 0;
}

Adapter::~Adapter()
{
    StopPacketDriver();
    m_pNILayer = nullptr;
}

bool Adapter::PacketStartDriver(pcap_if_t* pcap_if_t, int adtID)
{
    m_devName = pcap_if_t->name;
    m_description = pcap_if_t->description;
    m_macAddr = GetNICardAddress(pcap_if_t->name);
    m_adtID = adtID;
    char errbuf[PCAP_ERRBUF_SIZE];

    m_adapterHandler = pcap_open_live(pcap_if_t->name, 65536, 1, 2000, errbuf);
    m_thrdSwitch = TRUE;

    m_pThread = AfxBeginThread(ReadingThread, this);
    return true;
}

bool Adapter::Send(unsigned char* payload_data, int payload_data_len)
{
    if (pcap_sendpacket(m_adapterHandler, payload_data, payload_data_len) == -1) {
        char* err = pcap_geterr(m_adapterHandler);
        AfxMessageBox(_T("Packet Send Failed"));
        AfxMessageBox(_T(err));
        return FALSE;
    }
    return TRUE;
}
