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
    arpRequest = false;
    ResetARPHeader(INNER);
    ResetARPHeader(OUTER);
    //ResetIPHeader();
}

CIPLayer::~CIPLayer()
{
    
}

void CIPLayer::ResetTemp() {
    memset(m_temp.target_ip, 0, 4);
    memset(m_temp.target_mac, 0, 6);
    m_temp.check = false;
}

void CIPLayer::SetInterfaceInfo(unsigned char* macAddr1, unsigned char* ipAddr1, unsigned char* macAddr2, unsigned char* ipAddr2) {
    memcpy(interfaces[0].macAddr, macAddr1, 6);    // 내부 MAC 주소 설정
    memcpy(interfaces[0].ipAddr, ipAddr1, 4);      // 내부 IP 주소 설정
    memcpy(interfaces[1].macAddr, macAddr2, 6);    // 외부 MAC 주소 설정
    memcpy(interfaces[1].ipAddr, ipAddr2, 4);      // 외부 IP 주소 설정
}

void CIPLayer::SetTargetInfo(const unsigned char* targetIp) {
    memcpy(m_temp.target_ip, targetIp, 4);                 // 타겟 IP 주소 설정
}

unsigned char* CIPLayer::CheckProxyTable(const unsigned char* destIp, int io) {
    // destIp를 문자열 형식으로 변환
    std::string destIpStr = Converter::B2IP(destIp);

    // ProxyTable에서 IP로 엔트리 검색
    ProxyEntry* entry = proxyTable.FindEntryByIP(destIp);

    if (entry != nullptr) {
        // MAC 주소를 반환
        return entry->MACAddress;
    }

    // 일치하는 항목이 없으면 nullptr 반환
    return nullptr;
}

unsigned char* CIPLayer::CheckArpTable(const unsigned char* destIp, int io) {
    // 목적지 IP를 문자열로 변환
    std::string strIP = ARPCacheTable::binaryToString(destIp);

    // ARP 캐시 테이블에서 해당 IP의 MAC 주소를 검색
    auto it = cache.find(strIP); // cache는 상속받은 멤버 변수
    if (it == cache.end()) {
        // 해당 IP가 ARP 캐시 테이블에 없으면
        AfxMessageBox(_T("MAC 주소를 찾을 수 없습니다"), MB_ICONERROR | MB_OK);
        return nullptr;
    }

    // ARPCacheEntry를 통해 MAC 주소 가져오기
    ARPCacheEntry* entry = it->second;

    // MAC 주소 반환
    return (unsigned char*)entry->getMAC().c_str();
}

BOOL CIPLayer::IpSetEhternetAddr(unsigned char* srcMac, unsigned char* dstMac, int io) {
    ((CEthernetLayer*)GetUnderLayer())->SetDestinAddress(dstMac, io);
    ((CEthernetLayer*)GetUnderLayer())->SetSourceAddress(srcMac, io);
    return true;
}

unsigned char* CIPLayer::Routing(unsigned char* ip) {
    Fields entry = routingTable.findEntry(ip);
    if (entry.m_flag == e_flag::none) {
        return nullptr;
    }
    return entry.m_gateway;
}

BOOL CIPLayer::IpReceive(unsigned char* payload_data) {
    // 패킷에서 목적지 IP 주소 가져오기
    // **해당 IP 주소로 패킷 생성 후 보내기**
    // 1. Routing 있음 해당 Mac, 없으면 기본게이트웨이
    // 2. ARP 테이블에서 IP주소에 해당하는 MAC주소 찾기
    //    &Proxy 테이블에서 찾기
    //      찾았을때     : 해당 MAC 주소로 보넴
    //      못 찾았을때  : ARP request 후 reply MAC 주소로 보내기

    PIP_HEADER data = (PIP_HEADER)payload_data;

   if ( data->protocol_field != 1) return false;

    unsigned char srcMAC[6]; // interface의 MAC 주소 저장
    int io; //interface 번호

    //PIP_HEADER* data = reinterpret_cast<PIP_HEADER*>(payload_data);
    unsigned char* srcMAC_ip = Routing(data->dest_ip); //Source MAC주소를 해당 NI Card MAC주소로 바꾸려고 가져옴

    for (int i = 0; i < 2; i++) {
        if (memcmp(srcMAC_ip, interfaces[i].ipAddr, 4) == 0) { //둘이 같으면
            memcpy(srcMAC, interfaces[i].macAddr, 6);
            io = i; //interface 번호 저장
            break;
        }
    }

    unsigned char* destIp = data->dest_ip;
    unsigned char* destMAC = CheckArpTable(destIp, io);

    if (destMAC == nullptr) { //ARP cache table에서 해당 ip 주소가 없을 때
        destMAC = CheckProxyTable(destIp, io);

        if (destMAC == nullptr) { //Proxy table에서 찾아보고 없으면,
            // ARP requst보낸 후 ARP reply를 제대로 받은 경우에
            memcpy(m_temp.target_ip, data->dest_ip, 4); // arpReceive에서 참고할 target_IP 초기화.

            // ARP 요청 생성 및 전송
            createArpRequestPacket(destIp, io);
            // ARP 요청이 성공적으로 전송되었음을 알림
            // AfxMessageBox(_T("ARP 요청 전송 완료"), MB_ICONINFORMATION | MB_OK);
                
            while (!m_temp.check); 
            // check가 false이면, 대기 
            // check가 true이면 아래 코드 수행.
            // arpReceive에서 올바르게 수행되었을 때, true로 바뀜

            memcpy(destMAC, m_temp.target_mac, 6);
            // arp Receive에서 찾아낸 mac주소를 destMAC에 넣어주기.

            if (!IpSetEhternetAddr(srcMAC, destMAC, io)) {
                AfxMessageBox(_T("EthernetAddr 설정 실패"), MB_ICONERROR | MB_OK);
                return false;
            }
        }
        else { //Proxy table에 있으면 거기서 가져옴
            if (!IpSetEhternetAddr(srcMAC, destMAC, io)) {
                AfxMessageBox(_T("EthernetAddr 설정 실패"), MB_ICONERROR | MB_OK);
                return false;
            }
        }
    }
    else { //ARP cache table에 해당 ip주소를 찾았을 때
        if (!IpSetEhternetAddr(srcMAC, destMAC, io)) {
            AfxMessageBox(_T("EthernetAddr 설정 실패"), MB_ICONERROR | MB_OK);
            return false;
        }
    }

    if (IpSend(payload_data, IP_HEADER_SIZE + ICMP_HEADER_SIZE + ICMP_DATA_SIZE, io)) ResetTemp();
    return true;
}

BOOL CIPLayer::IpSend(unsigned char* ppayload, int nlength, int io) {
    BOOL success = ((CEthernetLayer*)(this->GetUnderLayer()))->Send(
        ppayload
        , IP_HEADER_SIZE + ICMP_HEADER_SIZE + ICMP_DATA_SIZE
        , IP_LAYER_IDENTIFIER, io);
    if (success) {
        //AfxMessageBox(_T("IP 패킷 전송 - IP Send"));
    }
    else {
        //AfxMessageBox(_T("IP 패킷 전송 실패 - IP Send"));
    }
    return success;
}

/////////////////////////////////////////////////////////////////

void CIPLayer::ResetARPHeader(int io)
{
    // 이더넷 목적지 주소, 나의 주소, 타입, Data를 초기화함
    arpHeader[io].hard_type = TO_BIG_ENDIAN_16(0x0001);    // Ethernet (1)
    arpHeader[io].prot_type = TO_BIG_ENDIAN_16(0x0800);    // IPv4 (0x0800)
    arpHeader[io].mac_len = 6;
    arpHeader[io].ip_len = 4;
    arpHeader[io].op_code = 0;
    memset(arpHeader[io].source_mac, 0, 6);
    memset(arpHeader[io].source_ip, 0, 4);
    memset(arpHeader[io].target_mac, 0, 6);
    memset(arpHeader[io].target_ip, 0, 4);
}

//void CIPLayer::SetSenderMac(const unsigned char* macAddress) {
//    memcpy(sender_mac, macAddress, 6);            // MAC 주소 설정
//}

BOOL CIPLayer::createArpRequestPacket(unsigned char* target_ip, int io) {
    // 없으면 브로드캐스트로 전송
    add(target_ip, interfaces[io].macAddr, false, false);
    ResetARPHeader(io);
    ((Cipc2023Dlg*)this->GetUpperLayer(0))->UpdateARPTable();
    memcpy(arpHeader[io].source_mac, interfaces[io].macAddr, 6);
    memcpy(arpHeader[io].source_ip, interfaces[io].ipAddr, 4);
    memcpy(arpHeader[io].target_ip, target_ip, 4);
    arpRequest = true;
    return createArpPacket(0x0001, io);
};

BOOL CIPLayer::createArpReplyPacket(unsigned char* payload_data, int io) {
    // 송신측의 맥주소와 IP주소를 타겟으로 바꾸고 송신측엔 나의 정보로 채움
    // target ip 주소가 나인지 확인
    // 내가 맞을 경우 source 필드를 target 필드로 설정하고
    // source 필드에 나의 ip주소 mac주소 설정
    // op code 2로 send에 전달
    PARP_HEADER data = (PARP_HEADER)payload_data;
    ResetARPHeader(io);

    // Target 필드 설정: Source 필드를 Target 필드로 설정
    memcpy(arpHeader[io].target_mac, data->source_mac, data->mac_len);  // source_mac -> target_mac
    memcpy(arpHeader[io].target_ip, data->source_ip, data->ip_len);   // source_ip -> target_ip

    // Source 필드 설정:
    memcpy(arpHeader[io].source_mac, interfaces[io].macAddr, data->mac_len);
    memcpy(arpHeader[io].source_ip, data->target_ip, data->ip_len);

    return createArpPacket(0x0002, io);
};

BOOL CIPLayer::createGarpPacket(int io) {
    // ARP Header
    // Source IP		: Sender's IP
    // Destination IP	: Sender's IP
    // Source Mac		: Sender's Mac
    // Destination Mac	: Broadcast

    // Ethernet
    // Source Mac		: Sender's Mac
    // Destination Mac	: Broadcast

    //const unsigned char broadcast_mac[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    //const unsigned char zero_ip[4] = { 0x00, 0x00, 0x00, 0x00 };

    //SetEthernetSrc(io);
    ResetARPHeader(io);
    memcpy(arpHeader[io].source_mac, interfaces[io].macAddr, 6);
    memcpy(arpHeader[io].source_ip, interfaces[io].ipAddr, 4);
    memcpy(arpHeader[io].target_ip, interfaces[io].ipAddr, 4);
    memcpy(arpHeader[io].target_mac, broadcast_mac, 6);
    return createArpPacket(0x0001, io);
}

BOOL CIPLayer::createArpPacket(unsigned short op_code, int io) {
    arpHeader[io].op_code = TO_BIG_ENDIAN_16(op_code);
    SetEthernetDst(arpHeader[io].target_mac, io);
    SetEthernetSrc(io);
    return ArpSend((unsigned char*)&arpHeader[io], ARP_HEADER_SIZE, io);
};

BOOL CIPLayer::ArpSend(unsigned char* ppayload, int nlength, int io) {   

    BOOL success = ((CEthernetLayer*)(this->GetUnderLayer()))->Send(ppayload, nlength, ARP_LAYER_IDENTIFIER, io);  

    if (success) {
        //AfxMessageBox(_T("ARP 패킷 전송 - ARP Send"));
    }
    else {
        //AfxMessageBox(_T("ARP 패킷 전송 실패 - ARP Send"));
    }
    return success;
}

BOOL CIPLayer::ArpReceive(unsigned char* payload_data, int io)
{
    PARP_HEADER data = (PARP_HEADER)payload_data;
    // ARP OP code가 1 - ARP 응답 패킷 생성 함수 호출
    unsigned short op = TO_BIG_ENDIAN_16(data->op_code);

    if (op == 1) {
        //update(data->source_ip, data->source_mac, true, true); // 이미 존재하면 수정
        if (add(data->source_ip, data->source_mac, true, true) ) {
            ((Cipc2023Dlg*)this->GetUpperLayer(0))->UpdateARPTable();   // dlg 업데이트
        }
        // 타겟 ip 주소가 나의 ip 주소와 같은지 or 프록시 테이블에 존재하는지
        if (memcmp(data->target_ip, interfaces[io].ipAddr, data->ip_len) == 0 || proxyTable.FindEntryByIP(data->target_ip)) {
            return createArpReplyPacket(payload_data, io); // 수정필요
        }
        return TRUE;
    }
    // ARP OP code가 2 - ARP cashe table 업데이트
    else if (op == 2) {
        if (!arpRequest) return false; // false여도 되나?
        
        bool isMine = false;
        for (int i = 0; i < 2; i++) {
            if (memcmp(data->target_ip, interfaces[i].ipAddr, data->ip_len) == 0) isMine = true;
        }
        if (!isMine) return false;

        //addOrUpdate(data->source_ip, data->source_mac, true, true);
        handleArpReply(data->source_ip);                            // incomplete->complete
        editEntryMacAddress(data->source_ip, data->source_mac);   // mac 주소 변경, 해당 ip 주소가 없으면 exception 발생
        ((Cipc2023Dlg*)this->GetUpperLayer(0))->UpdateARPTable();   // dlg 업데이트
        if (memcmp(m_temp.target_ip, data->source_ip, 4) == 0) {
            // ip 한테 mac 주소 받았다고 알림
            memcpy(m_temp.target_mac, data->source_mac, 6);
            m_temp.check = true;
        }
        return TRUE;
    }
}

void CIPLayer::onEntryTimeout(const unsigned char* ip) {
    std::string strIP = binaryToString(ip);
    removeEntry(ip);
    ((Cipc2023Dlg*)this->GetUpperLayer(0))->UpdateARPTable();
}

void CIPLayer::SetEthernetDst(unsigned char* target_mac, int io) {

    if (arpHeader[io].op_code == 1) {               // ARP request
        ((CEthernetLayer*)GetUnderLayer())->SetDestinAddress((unsigned char*)broadcast_mac, 0);
    }
    else if (arpHeader[io].op_code == 2) {          // ARP reply
        ((CEthernetLayer*)GetUnderLayer())->SetDestinAddress(target_mac, 0);
    }
}

void CIPLayer::SetEthernetSrc(int io) {
    ((CEthernetLayer*)GetUnderLayer())->SetSourceAddress(interfaces[io].macAddr, io);
}

//CString arpentries = _T("IP: 192.168.0.1, MAC: 00:1A:2B:3C:4D:5E, State: complete\r\n");
//unsigned char ipa[4] = { 192,168,0,1 };
//unsigned char maca[6] = { 0x00,0x1A,0x2B,0x3C,0x4D,0x5E };
//m_IP->addOrUpdate(ipa,maca,true,true);