#pragma once
#include "BaseLayer.h"
#include "utils.h"
#include <string>
#include <pcap.h>
#include "pch.h"
#include "NILayer.h"
#include "RoutingList.h"

class CNILayer;

class Adapter {
private:
	CNILayer* m_pNILayer;
	pcap_t* m_adapterHandler;
	std::string m_devName;
	std::string m_description;
	std::string m_macAddr;
	int m_adtID;
	bool m_thrdSwitch;
	CWinThread* m_pThread;
	// AfxBeginThread에 대한 포인터임. 이후 stopPacketDriver에서 이를 바탕으로 쓰레드 종료 예정

	bool StopPacketDriver();
	// 내부적으로 쓰레드 종료 -> pcap_close.
	std::string GetNICardAddress(char* adapter_name);
	static UINT		ReadingThread(LPVOID pParam);

public:
	Adapter(CNILayer* pParent)
		: m_pNILayer(pParent), m_adapterHandler(nullptr), m_adtID(-1), m_thrdSwitch(FALSE) {}
	// string 객체는 자동으로 빈문자열로 초기화가 진행된다.

	~Adapter(); // stopPacketDriver 작동.

	// 아래는 getter
	pcap_t* getHandler() { return m_adapterHandler; }
	std::string getDevName() { return m_devName; }
	std::string getDescription() { return m_description; }
	std::string getMacAddr() { return m_macAddr; }
	int getAdtId() { return m_adtID; }
	bool initAdapter(pcap_if_t* pcap_if_t, int adtID);
	bool PacketStartDriver();
	/*
	1. NILayer에서는 setAdapterList를 통해 장치 목록을 얻고, adtID가 주어지면 adtID를 기반으로 Adapters 벡터의 요소 중 하나에 접근
	2. 접근 후에 해당되는 pcap_if_t*를 PacketStartDriver에 넘김
	3. NILayer에서 pcap_if_t*를 인자로 주면, 그를 바탕으로 멤버변수에 추가 시작.
	*/
	bool Send(unsigned char* payload_data, int payload_data_len);
	// 각 Adapter 인스턴스마다 send를 가지므로 adtID를 가지고 Adapter를 선택 -> 해당 인스턴스의 send로 보내면 됨.
};