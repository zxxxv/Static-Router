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
#include "Adapter.h"
#include <vector>
#pragma comment (lib, "iphlpapi.lib")


class CNILayer
	: public CBaseLayer
{
protected:

	 
public:
	Adapter     GetAdapterObject(int iIndex); //
	void			SetAdapterIndex(int index);
	void			SetAdapterList();

	BOOL			Receive(unsigned char* payload_data, int adtID);
	BOOL			Send(unsigned char* payload_data, int payload_data_len);

	CNILayer(char* pName, int iNumAdapter = 0);
	virtual ~CNILayer();

	pcap_if_t* m_pAdapterList[NI_COUNT_NIC];
	std::vector<Adapter> m_adapters;

protected:
	int			m_iNumAdapter;
	int			m_index;
	// int			m_tempAdtID;
};

#endif // !defined(AFX_NILayer_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
