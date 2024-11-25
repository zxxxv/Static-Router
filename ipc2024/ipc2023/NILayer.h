// NILayer.h: interface for the CNILayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NILayer_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
#define AFX_NILayer_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include <pcap.h>
#include <Packet32.h>
#include <iphlpapi.h>
#pragma comment (lib, "iphlpapi.lib")

class CNILayer
	: public CBaseLayer
{
protected:
	pcap_t* m_AdapterObject;

public:
	BOOL			m_thrdSwitch;
	unsigned char* m_ppayload;

	void			PacketStartDriver(int index);
	BOOL			StopPacketDriver();

	pcap_if_t*      GetAdapterObject(int iIndex);
	void			SetAdapterIndex(int index);
	void			SetAdapterList();

	static UINT		ReadingThread(LPVOID pParam);

	BOOL			Receive(unsigned char* payload_data);
	BOOL			Send(unsigned char* payload_data, int payload_data_len);

	CString GetNICardAddress(char* adapter_name);

	CNILayer(char* pName, pcap_t* pAdapterObject = NULL, int iNumAdapter = 0);
	virtual ~CNILayer();

	pcap_if_t* m_pAdapterList[NI_COUNT_NIC];

protected:
	int			m_iNumAdapter;
	int			m_index;
};

#endif // !defined(AFX_NILayer_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
