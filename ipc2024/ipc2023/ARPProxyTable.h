#pragma once

#include "pch.h"
#include <map>
#include <array>
#include <string>
#include <afxwin.h>
#include <sstream>

struct ProxyEntry {
    CString deviceName;
    unsigned char MACAddress[6]; // MAC 주소
};

class ARPProxyTable
{
public:

    static ARPProxyTable& GetInstance() {
        static ARPProxyTable instance;  // 유일한 인스턴스
        return instance;
    }

    // 항목 추가
    void AddEntry(const CString& deviceName, const unsigned char ipAddress[4], const unsigned char macAddress[6]);

    // 모든 항목 삭제
    void ClearTable();

    // IP 주소로 항목 검색
    ProxyEntry* FindEntryByIP(const unsigned char* ipAddress);

    // 캐시 테이블의 모든 항목 반환
    const std::map<std::string, ProxyEntry>& GetAllEntries() const;

    // 전달한 ip 주소에 해당하는 항목 삭제
    BOOL ARPProxyTable::RemoveEntryByIP(const unsigned char* ipAddress);

    // 모든 항목 메시지 박스로 띄움
    void DisplayAllEntries() const;

private:

    ARPProxyTable();
    ~ARPProxyTable();

    ARPProxyTable(const ARPProxyTable&) = delete;
    ARPProxyTable& operator=(const ARPProxyTable&) = delete;

    std::map<std::string, ProxyEntry> m_entries; // Proxy ARP 테이블을 위한 맵
    std::string IpToString(const unsigned char* ipAddress) const;
};