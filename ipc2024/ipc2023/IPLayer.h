#pragma once
#include "BaseLayer.h"
#include "ARPCacheTable.h"
#include "pch.h"
#include "ARPProxyTable.h"


class CIPLayer : public CBaseLayer, public ARPCacheTable
{

public:

    void UpdateEthernetDestMac(const unsigned char* destIp);

    /////////////////////// ARP
    CIPLayer(char* name);
    virtual ~CIPLayer();

    // ARP 요청 패킷 생성 함수
    void createRequestPacket(int index);

    // ARP 응답 패킷 생성 함수
    void createReplyPacket(unsigned char* payload_data, int index);

    // ARP 패킷을 전송하는 함수
    BOOL Send(unsigned char* ppayload, int nlength);

    // 수신한 ARP 패킷을 처리하는 함수
    BOOL Receive(unsigned char* payload_data, int index);

    void createPacket(unsigned short op_code);

    BOOL SetEthernetDest(unsigned char* target_mac);

    //dlg에서 나의 맥, 아이피
    void CIPLayer::SetInterfaceInfo(unsigned char* macAddr1, unsigned char* ipAddr1, unsigned char* macAddr2, unsigned char* ipAddr2);

    //void SetSenderMac(const unsigned char* macAddress);

    //dlg에서 브로드캐스트 보낼 ip 받아오는 함수
    void SetTargetInfo(const unsigned char* target_ip);

    void onEntryTimeout(const unsigned char* ip) override;
    /*
    onEntryTimeout: 타임아웃된 항목을 삭제하는 함수. Entry가 타임아웃되면 트리거된다.
        # parameter
        - binary ip 주소, 각 entry의 OnTimer 함수 내에서 입력된다.
    */

    BOOL CIPLayer::createGarpPacket(int index);


    //unsigned char sender_mac[6];  // MAC 주소를 저장하는 변수
    //unsigned char sender_ip[4];   // 나의 IP 주소를 저장하는 변수
    unsigned char target_ip[4];   // 타겟 IP 주소를 저장하는 변수
    ARPProxyTable& proxyTable = ARPProxyTable::GetInstance();
    
    typedef struct _INTERFACE_CARD {

        unsigned char macAddr[6];
        unsigned char ipAddr[4];

    } INTERFACE_CARD;

    // 총 28bytes
    typedef struct _APR_HEADER {
        unsigned short   hard_type;    // hardware type 이더넷 소통이므로 1 (2 bytes)
        unsigned short   prot_type;    // protocol type (2 bytes)
        unsigned char   mac_len;    // hardware address length (1 byte)
        unsigned char   ip_len;    // protocol address length (1 byte)
        unsigned short   op_code;    // 오퍼레이션 코드 (2 bytes)
        unsigned char   source_mac[6];      // source hardware address (6 bytes)
        unsigned char   source_ip[4];      // source protocol address (4 bytes)
        unsigned char   target_mac[6];      // target hardware address (6 bytes)
        unsigned char   target_ip[4];      // target protocol address (4 bytes)
    } ARP_HEADER, * PARP_HEADER;
private:
    void ResetARPHeader();
    INTERFACE_CARD interfaces[2];

#define ARP_HEADER_SIZE 28
#define ARP_LAYER_IDENTIFIER 0x0806

protected:
    ARP_HEADER   arpHeader;   /// 객체 ARP 해더

    std::unordered_map<std::string, ARPCacheEntry*> cache;
};