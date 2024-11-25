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
    ResetARPHeader();
}

CIPLayer::~CIPLayer()
{

}

// ************목적지 IP를 기준으로 Ethernet 헤더의 목적지 MAC 주소를 업데이트하는 함수*********************
void CIPLayer::UpdateEthernetDestMac(const unsigned char* destIp) {
    // 목적지 IP를 문자열로 변환
    std::string strIP = ARPCacheTable::binaryToString(destIp);

    // ARP 캐시 테이블에서 해당 IP의 MAC 주소를 검색
    auto it = cache.find(strIP); // cache는 상속받은 멤버 변수
    if (it == cache.end()) {
        // 해당 IP가 ARP 캐시 테이블에 없으면
        AfxMessageBox(_T("MAC 주소를 찾을 수 없습니다"), MB_ICONERROR | MB_OK);
        return;
    }

    // ARPCacheEntry를 통해 MAC 주소 가져오기
    ARPCacheEntry* entry = it->second;

    // MAC 주소를 바이트 배열로 변환
    unsigned char mac[6];
    memcpy(mac, entry->getMAC().c_str(), 6);

    // EthernetLayer의 SetDestinAddress를 사용해 목적지 MAC 주소를 설정
    ((CEthernetLayer*)GetUnderLayer())->SetDestinAddress(mac, 0);
}

/////////////////////////////////////////////////////////////////ARP

void CIPLayer::ResetARPHeader()
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

void CIPLayer::SetInterfaceInfo(unsigned char* macAddr1, unsigned char* ipAddr1, unsigned char* macAddr2, unsigned char* ipAddr2) {
    memcpy(interfaces[0].macAddr, macAddr1, 6);    // 내부 MAC 주소 설정
    memcpy(interfaces[0].ipAddr, ipAddr1, 4);      // 내부 IP 주소 설정
    memcpy(interfaces[1].macAddr, macAddr2, 6);    // 외부 MAC 주소 설정
    memcpy(interfaces[1].ipAddr, ipAddr2, 4);      // 외부 IP 주소 설정
}

//void CIPLayer::SetSenderMac(const unsigned char* macAddress) {
//    memcpy(sender_mac, macAddress, 6);    // MAC 주소 설정
//}

void CIPLayer::SetTargetInfo(const unsigned char* targetIp) {
    memcpy(target_ip, targetIp, 4);       // 타겟 IP 주소 설정
}

BOOL CIPLayer::SetEthernetDest(unsigned char* target_mac) {
    const unsigned char broadcast_mac[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    if (arpHeader.op_code == 1) {  // ARP request
        ((CEthernetLayer*)GetUnderLayer())->SetDestinAddress((unsigned char*)broadcast_mac, 0);
    }
    else if (arpHeader.op_code == 2) {  // ARP reply
        ((CEthernetLayer*)GetUnderLayer())->SetDestinAddress(target_mac, 0);
    }
    return true;
}

void CIPLayer::createRequestPacket(int index) {
    // 선택된 IP주소에 해당하는 mac주소가 있으면 전송 X
    // 없으면 브로드캐스트로 전송
    unsigned char defaultMac[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    if (addOrUpdate(target_ip, defaultMac, false, false)) {
        ResetARPHeader();
        memcpy(arpHeader.source_mac, interfaces[index].macAddr, 6);
        memcpy(arpHeader.source_ip, interfaces[index].ipAddr, 4);
        memcpy(arpHeader.target_ip, target_ip, 4);

        createPacket(0x0001);
    }
}

BOOL CIPLayer::createGarpPacket(int index) {
    // ARP Header
    // Source IP		: Sender's IP
    // Destination IP	: Sender's IP
    // Source Mac		: Sender's Mac
    // Destination Mac	: Broadcast

    // Ethernet
    // Source Mac		: Sender's Mac
    // Destination Mac	: Broadcast

    const unsigned char broadcast_mac[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    //const unsigned char zero_ip[4] = { 0x00, 0x00, 0x00, 0x00 };

    ((CEthernetLayer*)GetUnderLayer())->SetSourceAddress(interfaces[index].macAddr, 0);

    ResetARPHeader();

    memcpy(arpHeader.source_mac, interfaces[index].macAddr, 6);
    memcpy(arpHeader.source_ip, interfaces[index].ipAddr, 4);
    memcpy(arpHeader.target_ip, interfaces[index].ipAddr, 4);
    memcpy(arpHeader.target_mac, broadcast_mac, 6);

    createPacket(0x0001);

    return TRUE;
}

void CIPLayer::createReplyPacket(unsigned char* payload_data, int index) {
    // 송신측의 맥주소와 IP주소를 타겟으로 바꾸고 송신측엔 나의 정보로 채움
    // target ip 주소가 나인지 확인
    // 내가 맞을 경우 source 필드를 target 필드로 설정하고
    // source 필드에 나의 ip주소 mac주소 설정
    // op code 2로 send에 전달
    PARP_HEADER data = (PARP_HEADER)payload_data;
    ResetARPHeader();

    // Target 필드 설정: Source 필드를 Target 필드로 설정
    memcpy(arpHeader.target_mac, data->source_mac, data->mac_len);  // source_mac -> target_mac
    memcpy(arpHeader.target_ip, data->source_ip, data->ip_len);   // source_ip -> target_ip

    // Source 필드 설정:
    memcpy(arpHeader.source_mac, interfaces[index].macAddr, data->mac_len);
    memcpy(arpHeader.source_ip, data->target_ip, data->ip_len);

    createPacket(0x0002);
};

void CIPLayer::createPacket(unsigned short op_code) {
    arpHeader.op_code = TO_BIG_ENDIAN_16(op_code);
    SetEthernetDest(arpHeader.target_mac);
    Send((unsigned char*)&arpHeader, ARP_HEADER_SIZE);
};

BOOL CIPLayer::Send(unsigned char* ppayload, int nlength)
{
    BOOL success = ((CEthernetLayer*)(this->GetUnderLayer()))->Send(ppayload, ARP_HEADER_SIZE, ARP_LAYER_IDENTIFIER, 0);  // ARP 패킷 타입 0x0806

    if (success) {
        AfxMessageBox(_T("패킷 전송 성공 - ARP Send"));
    }
    else {
        AfxMessageBox(_T("패킷 전송 실패 - ARP Send"));
    }
    return success;
}

BOOL CIPLayer::Receive(unsigned char* payload_data, int index)
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
        if (memcmp(data->target_ip, interfaces[index].ipAddr, data->ip_len) == 0 || proxyTable.FindEntryByIP(data->target_ip)) {
            createReplyPacket(payload_data, 0); // 수정필요
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