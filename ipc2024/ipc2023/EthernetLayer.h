// EthernetLayer.h: interface for the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
#define AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "pch.h"

class CEthernetLayer
    : public CBaseLayer
{
public:
    BOOL         Receive(unsigned char* payload_data_len, int io);
    BOOL         Send(unsigned char* payload_data, int payload_data_len, unsigned short type, int io);
    void         SetDestinAddress(unsigned char* pAddress, int io);
    void         SetSourceAddress(unsigned char* pAddress, int io);
    void         SetInterfaceInfo(unsigned char* macAddr1, unsigned char* macAddr2);
    /*unsigned char* GetDestinAddress();
    unsigned char* GetSourceAddress();*/

    CEthernetLayer(char* pName);
    virtual ~CEthernetLayer();

    typedef struct _INTERFACE_CARD {
        unsigned char macAddr[6];
    } INTERFACE_CARD;

    typedef struct _ETHERNET_HEADER {

        unsigned char   enet_dstaddr[6];                // destination address of ethernet layer
        unsigned char   enet_srcaddr[6];                // source address of ethernet layer
        unsigned short  enet_type;                      // type of ethernet layer
        unsigned char   enet_data[ETHER_MAX_DATA_SIZE]; // frame data

    } ETHERNET_HEADER, * PETHERNET_HEADER;

private:
    inline void         ResetHeader(int io);
    unsigned char       broadcastAddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    INTERFACE_CARD interfaces[2];

protected:
    ETHERNET_HEADER   m_sHeader[2];   /// 객체 이더넷 해더
};

#endif // !defined(AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
