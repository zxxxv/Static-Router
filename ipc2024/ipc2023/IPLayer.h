#pragma once
#include "BaseLayer.h"
#include "ARPCacheTable.h"
#include "pch.h"
#include "ARPProxyTable.h"


class CIPLayer : public CBaseLayer, public ARPCacheTable
{

public:
    CIPLayer(char* name);
    virtual ~CIPLayer();

    /////////////////////// IP
    
    //목적지 IP를 기준으로 Ethernet 헤더의 목적지 MAC 주소를 업데이트하는 함수
    BOOL UpdateEthernetDestMac(const unsigned char* destIp, int io);

    // 수신한 IP 패킷 처리하는 함수
    BOOL IpReceive(unsigned char* payload_data, int io);
    BOOL IpSend(unsigned char* ppayload, int nlength, int io);
    BOOL CheckProxyTable(const unsigned char* destIp, int io);
    unsigned char* Routing(unsigned char* ip);

    /////////////////////// ARP
    // ARP 요청 패킷 생성 함수
    BOOL createArpRequestPacket(unsigned char* target_ip, int io);
    /*
    createArpRequestPacket: 궁금한 mac 주소에 해당하는 ip 주소 입력 및 어댑터(내부, 외부) 선택
    내부 입력 -> INNER == 0
    외부 입력 -> OUTER == 1
    */

    // ARP 응답 패킷 생성 함수
    BOOL createArpReplyPacket(unsigned char* payload_data, int io);

    // ARP 패킷을 전송하는 함수
    BOOL ArpSend(unsigned char* ppayload, int nlength, int io);

    // 수신한 ARP 패킷을 처리하는 함수
    BOOL ArpReceive(unsigned char* payload_data, int io);

    BOOL createArpPacket(unsigned short op_code, int io);

    void SetEthernetDst(unsigned char* target_mac, int io);
    void SetEthernetSrc(int io);

    //dlg에서 설정한 mac, ip
    void CIPLayer::SetInterfaceInfo(unsigned char* macAddr1, unsigned char* ipAddr1, unsigned char* macAddr2, unsigned char* ipAddr2);

    //void SetSenderMac(const unsigned char* macAddress);

    //dlg에서 브로드캐스트 보낼 ip 받아오는 함수
    //void SetTargetInfo(const unsigned char* target_ip);

    void onEntryTimeout(const unsigned char* ip) override;
    /*
    onEntryTimeout: 타임아웃된 항목을 삭제하는 함수. Entry가 타임아웃되면 트리거된다.
        # parameter
        - binary ip 주소, 각 entry의 OnTimer 함수 내에서 입력된다.
    */

    BOOL CIPLayer::createGarpPacket(int io);

    unsigned char target_ip[4];   // 타겟 IP 주소를 저장하는 변수
    ARPProxyTable& proxyTable = ARPProxyTable::GetInstance();
    RoutingList& routingTable = RoutingList::GetInstance();
    
    typedef struct _INTERFACE_CARD {

        unsigned char macAddr[6];
        unsigned char ipAddr[4];

    } INTERFACE_CARD;

    // 총 60 bytes = IP 헤더 20 bytes + ICMP 헤더 8 bytes + ICMP data 32 bytes
    typedef struct _IP_HEADER {
        unsigned char       version_ihl;                                // 버전(4비트) 
        unsigned char       header_length;                              // 헤더 길이(4비트)
        unsigned char       tos;                                        // 서비스 타입 (1 bytes)
        unsigned short      total_length;                               // 전체 패킷 길이 (2 bytes)
        unsigned short      identification_field;                       // 식별자 (2 bytes)
        unsigned short      unused;                                     // 3-bit flags + 13-bit fragment offset (2 bytes)
        unsigned char       ttl;                                        // TTL (1 bytes)
        unsigned char       protocol_field;                             // 프로토콜 (ICMP=1, TCP=6 등) (1 bytes)
        unsigned short      header_checksum;                            // 헤더 체크섬 (2 bytes)
        unsigned char       source_ip[4];                               // 출발지 IP 주소 (4 bytes)
        unsigned char       dest_ip[4];                                 // 목적지 IP 주소 (4 bytes)
        unsigned char       icmp[ICMP_HEADER_SIZE + ICMP_DATA_SIZE];    // ICMP 헤더 8 bytes + ICMP data 32 bytes
    } IP_HEADER, *PIP_HEADER;


    // 총 28 bytes
    typedef struct _APR_HEADER {
        unsigned short      hard_type;              // hardware type 이더넷 소통이므로 1 (2 bytes)
        unsigned short      prot_type;              // protocol type (2 bytes)
        unsigned char       mac_len;                // hardware address length (1 byte)
        unsigned char       ip_len;                 // protocol address length (1 byte)
        unsigned short      op_code;                // 오퍼레이션 코드 (2 bytes)
        unsigned char       source_mac[6];          // source hardware address (6 bytes)
        unsigned char       source_ip[4];           // source protocol address (4 bytes)
        unsigned char       target_mac[6];          // target hardware address (6 bytes)
        unsigned char       target_ip[4];           // target protocol address (4 bytes)
    } ARP_HEADER, * PARP_HEADER;
private:
    void ResetARPHeader(int io);
    INTERFACE_CARD interfaces[2];

    const unsigned char broadcast_mac[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    unsigned char defaultMac[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    unsigned char defaultIp[4] = { 0, 0, 0, 0 };

protected:
    ARP_HEADER   arpHeader[2];   /// 객체 ARP 해더

    std::unordered_map<std::string, ARPCacheEntry*> cache;
};

/*
INNER/OUTER

ARP_Request
1. createArpRequestPacket()
2. createArpPacket()

ARP_Reply
createArpReplyPacket()

GARP
1. createGarpPacket()
2. createArpPacket()
*/