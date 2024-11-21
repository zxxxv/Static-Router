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

    // ARP 요청 패킷 생성 함수
    void createRequestPacket();

    // ARP 응답 패킷 생성 함수
    void createReplyPacket(unsigned char* payload_data);

    // ARP 패킷을 전송하는 함수
    BOOL Send(unsigned char* ppayload, int nlength);

    // 수신한 ARP 패킷을 처리하는 함수
    BOOL Receive(unsigned char* payload_data);

    void createPacket(unsigned short op_code);

    BOOL SetEthernetDest(unsigned char* target_mac);

    //dlg에서 나의 맥, 아이피
    void SetSenderInfo(const unsigned char* sender_mac, const unsigned char* sender_ip);

    void SetSenderMac(const unsigned char* macAddress);

    //dlg에서 브로드캐스트 보낼 ip 받아오는 한수
    void SetTargetInfo(const unsigned char* target_ip);

    void onEntryTimeout(const unsigned char* ip) override;
    /*
    onEntryTimeout: 타임아웃된 항목을 삭제하는 함수. Entry가 타임아웃되면 트리거된다.
        # parameter
        - binary ip 주소, 각 entry의 OnTimer 함수 내에서 입력된다.
    */

    void CIPLayer::createGarpPacket(unsigned char* mac);

    unsigned char sender_mac[6];  // MAC 주소를 저장하는 변수
    unsigned char sender_ip[4];   // 나의 IP 주소를 저장하는 변수
    unsigned char target_ip[4];   // 타겟 IP 주소를 저장하는 변수
    ARPProxyTable& proxyTable = ARPProxyTable::GetInstance();
    //ARPProxyTable proxyTable;

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
    void ResetHeader();

#define ARP_HEADER_SIZE 28
#define ARP_LAYER_IDENTIFIER 0x0806

protected:
    ARP_HEADER   arpHeader;   /// 객체 ARP 해더

};