#include "stdafx.h"
#include "pch.h"
#include "IPLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define TO_BIG_ENDIAN_16(x) ((unsigned short)(((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8)))

CIPLayer::CIPLayer(char* pName)
    : CBaseLayer(pName)
{
    ResetHeader();
}

CIPLayer::~CIPLayer()
{

}

void CIPLayer::ResetHeader()
{
    // 이더넷 목적지 주소, 나의 주소, 타입, Data를 초기화함
    arpHeader.hard_type = TO_BIG_ENDIAN_16(0x0001);    // Ethernet (1)
    arpHeader.prot_type = TO_BIG_ENDIAN_16(0x0800);    // IPv4 (0x0800)
    arpHeader.mac_len = 6;
    arpHeader.ip_len = 4;
    arpHeader.op_code = 0;
    memset(arpHeader.source_mac, 6, 0);
    memset(arpHeader.source_ip, 4, 0);
    memset(arpHeader.target_mac, 6, 0);
    memset(arpHeader.target_ip, 4, 0);
}

void CIPLayer::SetSenderInfo(const unsigned char* macAddress, const unsigned char* ipAddress) {
    memcpy(sender_mac, macAddress, 6);    // MAC 주소 설정
    memcpy(sender_ip, ipAddress, 4);      // 나의 IP 주소 설정
}

void CIPLayer::SetSenderMac(const unsigned char* macAddress) {
    memcpy(sender_mac, macAddress, 6);    // MAC 주소 설정
}

void CIPLayer::SetTargetInfo(const unsigned char* targetIp) {
    memcpy(target_ip, targetIp, 4);       // 타겟 IP 주소 설정
}

BOOL CIPLayer::SetEthernetDest(unsigned char* target_mac) {
    const unsigned char broadcast_mac[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    if (arpHeader.op_code == 1) {  // ARP request
        ((CEthernetLayer*)GetUnderLayer())->SetDestinAddress((unsigned char*)broadcast_mac);
    }
    else if (arpHeader.op_code == 2) {  // ARP reply
        ((CEthernetLayer*)GetUnderLayer())->SetDestinAddress(target_mac);
    }
    return true;
}

void CIPLayer::createRequestPacket() {
    // 선택된 IP주소에 해당하는 mac주소가 있으면 전송 X
    // 없으면 브로드캐스트로 전송
    unsigned char defaultMac[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    if (addOrUpdate(target_ip, defaultMac, false, false)) {
        ResetHeader();
        memcpy(arpHeader.source_mac, sender_mac, 6);
        memcpy(arpHeader.source_ip, sender_ip, 4);
        memcpy(arpHeader.target_ip, target_ip, 4);

        createPacket(0x0001);
    }
}

void CIPLayer::createGarpPacket(unsigned char* mac) {
    // ARP Header
    // Source IP		: Sender's
    // Destination IP	: Sender's
    // Source Mac		: Sender's 변경된 Mac
    // Destination Mac	: Broadcast

    // Ethernet
    // Source Mac		: Sender's 변경된 Mac
    // Destination Mac	: Broadcast

    const unsigned char broadcast_mac[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    //const unsigned char zero_ip[4] = { 0x00, 0x00, 0x00, 0x00 };

    ResetHeader();

    memcpy(arpHeader.source_mac, mac, 6);
    memcpy(arpHeader.source_ip, sender_ip, 4);
    memcpy(arpHeader.target_ip, sender_ip, 4);
    memcpy(arpHeader.target_mac, broadcast_mac, 6);

    createPacket(0x0001);
}

void CIPLayer::createReplyPacket(unsigned char* payload_data) {
    // 송신측의 맥주소와 IP주소를 타겟으로 바꾸고 송신측엔 나의 정보로 채움
    // target ip 주소가 나인지 확인
    // 내가 맞을 경우 source 필드를 target 필드로 설정하고
    // source 필드에 나의 ip주소 mac주소 설정
    // op code 2로 send에 전달
    PARP_HEADER data = (PARP_HEADER)payload_data;
    ResetHeader();

    // Target 필드 설정: Source 필드를 Target 필드로 설정
    memcpy(arpHeader.target_mac, data->source_mac, data->mac_len);  // source_mac -> target_mac
    memcpy(arpHeader.target_ip, data->source_ip, data->ip_len);   // source_ip -> target_ip

    // Source 필드 설정:
    memcpy(arpHeader.source_mac, sender_mac, data->mac_len);
    memcpy(arpHeader.source_ip, data->target_ip, data->ip_len);

    createPacket(0x0002);
};

void CIPLayer::createPacket(unsigned short op_code) {
    arpHeader.op_code = op_code;
    SetEthernetDest(arpHeader.target_mac);
    Send((unsigned char*)&arpHeader, ARP_HEADER_SIZE);
};

BOOL CIPLayer::Send(unsigned char* ppayload, int nlength)
{
    BOOL success = ((CEthernetLayer*)(this->GetUnderLayer()))->Send(ppayload, ARP_HEADER_SIZE, ARP_LAYER_IDENTIFIER);  // ARP 패킷 타입 0x0806

    if (success) {
        AfxMessageBox(_T("패킷 전송 성공 - ARP Send"));
    }
    else {
        AfxMessageBox(_T("패킷 전송 실패 - ARP Send"));
    }
    return success;
}

BOOL CIPLayer::Receive(unsigned char* payload_data)
{
    PARP_HEADER data = (PARP_HEADER)payload_data;

    //받은 ARP OP code가 1 - ARP 응답 패킷 생성 함수 호출
    if (data->op_code == 1) {
        
        addOrUpdate(data->source_ip, data->source_mac, true, true); // 이미 존재하면 덮어씌우는 것으로 바꾸기
        // dlg 업데이트 하기
        unsigned char buffer[10];
        memcpy(buffer, data->source_mac, 6);  // source_mac 복사 (6 bytes)
        memcpy(buffer + 6, data->source_ip, 4);  // source_ip 복사 (4 bytes)
        mp_aUpperLayer[0]->Receive(buffer);

        // 타겟 ip 주소가 나의 ip 주소와 같은지 or 프록시 테이블에 존재하는지
        if (memcmp(data->target_ip, sender_ip, data->ip_len) == 0 || proxyTable.FindEntryByIP(data->target_ip)) {
            createReplyPacket(payload_data);
        }
    }
    //받은 ARP OP code가 2 - ARP cashe table 업데이트 
    else if (data->op_code == 2) {
        // ARP 캐시 테이블 업데이트
        handleArpReply(data->source_ip);
        
        // dlg 업데이트
        unsigned char buffer[10];
        memcpy(buffer, data->source_mac, 6);  // source_mac 복사 (6 bytes)
        memcpy(buffer + 6, data->source_ip, 4);  // source_ip 복사 (4 bytes)
        mp_aUpperLayer[0]->Receive(buffer);
    };

    return true;
}

void CIPLayer::onEntryTimeout(const unsigned char* ip) {
    std::string strIP = binaryToString(ip);
    ((Cipc2023Dlg*)this->GetUpperLayer(0))->TimeoutEntryDelete(ip);
    removeEntry(ip);
}