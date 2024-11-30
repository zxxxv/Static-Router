// EthernetLayer.cpp: implementation of the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch.h"
#include "EthernetLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define TO_BIG_ENDIAN_16(x) ((unsigned short)(((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8)))

CEthernetLayer::CEthernetLayer(char* pName)
    : CBaseLayer(pName)
{
    ResetHeader(INNER);
    ResetHeader(OUTER);
}

CEthernetLayer::~CEthernetLayer()
{

}

void CEthernetLayer::SetInterfaceInfo(unsigned char* macAddr1, unsigned char* macAddr2) {
    memcpy(interfaces[0].macAddr, macAddr1, 6);    // 내부 MAC 주소 설정
    memcpy(interfaces[1].macAddr, macAddr2, 6);    // 외부 MAC 주소 설정
}

void CEthernetLayer::ResetHeader(int io)
{
    // 이더넷 목적지 주소, 나의 주소, 타입, Data를 초기화함
    memset(m_sHeader[io].enet_dstaddr, 0, 6);
    memset(m_sHeader[io].enet_srcaddr, 0, 6);
    memset(m_sHeader[io].enet_data, 0, ETHER_MAX_DATA_SIZE);
    m_sHeader[io].enet_type = 0;
}

void CEthernetLayer::SetSourceAddress(unsigned char* pAddress, int io)
{
    // 받은 나의 주소를 이더넷 source 주소로 설정
    memcpy(m_sHeader[io].enet_srcaddr, pAddress, 6);
}

void CEthernetLayer::SetDestinAddress(unsigned char* pAddress, int io)
{
    // 받은 목적지 주소를 이더넷 destination 주소로 설정
    memcpy(m_sHeader[io].enet_dstaddr, pAddress, 6);
}

BOOL CEthernetLayer::Send(unsigned char* payload_data, int payload_data_len, unsigned short type, int io)
{
    // ChatApp 계층에서 받은 App 계층의 Frame 길이만큼 Ethernet계층의 data로 넣는다
    memcpy(m_sHeader[io].enet_data, payload_data, payload_data_len);
    m_sHeader[io].enet_type = TO_BIG_ENDIAN_16(type);
    //m_sHeader.enet_type = type;
    BOOL bSuccess = FALSE;

    // 만든 이더넷 data에 이더넷 헤드를 추가해서 NI 계층으로 보냄
    //bSuccess = mp_UnderLayer->Send((unsigned char*)&m_sHeader, payload_data_len + ETHER_HEADER_SIZE); //1514

    return bSuccess;
}

BOOL CEthernetLayer::Receive(unsigned char* payload_data, int io)
{
    // payload_data를 이더넷 헤더 구조체로 넣는다
    PETHERNET_HEADER pFrame = (PETHERNET_HEADER)payload_data;

    BOOL bSuccess = FALSE;
    
    // 목적지 주소가 나의 주소가 아니면서, 브로드 캐스트가 아니면 무시
    if (memcmp(pFrame->enet_dstaddr, interfaces[io].macAddr, 6) != 0 &&
        memcmp(pFrame->enet_dstaddr, broadcastAddr, 6) != 0)
        return FALSE;
    // 내가 보낸 값이 나에게 온건지
    if (memcmp(pFrame->enet_srcaddr, interfaces[io].macAddr, 6) == 0)
        return FALSE;

    unsigned short type = TO_BIG_ENDIAN_16(pFrame->enet_type);

    switch (type) {
        case ARP_LAYER_IDENTIFIER:
            return mp_aUpperLayer[0]->ArpReceive((unsigned char*)pFrame->enet_data, io);
            break;
        case IP_LAYER_IDENTIFIER:
            return mp_aUpperLayer[0]->IpReceive((unsigned char*)pFrame->enet_data, io);
            break;
    }
    return FALSE;
}