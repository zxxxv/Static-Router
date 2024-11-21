#include "stdafx.h"
#include "pch.h"
#include "ARPProxyTable.h"


ARPProxyTable::ARPProxyTable() {
    
}

ARPProxyTable::~ARPProxyTable() {
  
}

std::string ARPProxyTable::IpToString(const unsigned char* ipAddress) const {
    std::ostringstream oss{};
    oss << static_cast<int>(ipAddress[0]) << "."
        << static_cast<int>(ipAddress[1]) << "."
        << static_cast<int>(ipAddress[2]) << "."
        << static_cast<int>(ipAddress[3]);
    return oss.str();
}


void ARPProxyTable::AddEntry(const CString& deviceName, const unsigned char ipAddress[4], const unsigned char macAddress[6]) {
    ProxyEntry entry;
    entry.deviceName = deviceName;
    memcpy(entry.MACAddress, macAddress, sizeof(entry.MACAddress)); // MAC 주소 복사

    // IP 주소를 문자열로 변환하여 키로 사용
    std::string ipKey = IpToString(ipAddress);

    m_entries[ipKey] = entry;
}


// 모든 항목 삭제
void ARPProxyTable::ClearTable() {
    m_entries.clear();
}

// IP 주소 항목 검색
ProxyEntry* ARPProxyTable::FindEntryByIP(const unsigned char* ipAddress) {
    // IP 주소를 문자열로 변환하여 키로 사용
    std::string ipKey = IpToString(ipAddress);

    auto it = m_entries.find(ipKey);
    if (it != m_entries.end()) {
        return &(it->second);
    }
    return nullptr;  // 해당 IP 주소가 없음
}

// IP 주소 항목 삭제
BOOL ARPProxyTable::RemoveEntryByIP(const unsigned char* ipAddress) {
    // IP 주소를 문자열로 변환하여 키로 사용
    std::string ipKey = IpToString(ipAddress);
    auto it = m_entries.find(ipKey);
    if (it != m_entries.end()) {
        m_entries.erase(it);
        return true;
    }
    else {
        return false;
    }
}

// 모든 항목을 반환
const std::map<std::string, ProxyEntry>& ARPProxyTable::GetAllEntries() const {
    return m_entries;
}

// 모든 항목을 메시지 박스로 표시
void ARPProxyTable::DisplayAllEntries() const {
    CString message;
    for (const auto& entryPair : m_entries) {
        const std::string& ip = entryPair.first;
        const ProxyEntry& entry = entryPair.second;

        // MAC 주소를 문자열로 변환
        CString macAddress;
        macAddress.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"),
            entry.MACAddress[0], entry.MACAddress[1], entry.MACAddress[2],
            entry.MACAddress[3], entry.MACAddress[4], entry.MACAddress[5]);

        // 항목 정보를 문자열로 구성
        CString entryStr;
        entryStr.Format(_T("Device: %s\nIP: %s\nMAC: %s\n\n"),
            entry.deviceName, CString(ip.c_str()), macAddress);

        message += entryStr;
    }

    if (message.IsEmpty()) {
        message = _T("No entries found");
    }

    AfxMessageBox(message);
}