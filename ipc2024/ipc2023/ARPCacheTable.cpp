// ARPCacheTable.cpp
#include "pch.h"
#include "ARPCacheTable.h"

ARPCacheTable::ARPCacheTable() {
    // 필요한 초기화 작업을 수행할 수 있습니다.
    std::cout << "ARPCacheTable initialized." << std::endl;
}

ARPCacheTable::~ARPCacheTable() {
    for (auto& pair : cache) {
        delete pair.second;
    }
}

std::string ARPCacheTable::binaryToString(const unsigned char* ip) {
    return std::to_string(ip[0]) + "." +
        std::to_string(ip[1]) + "." +
        std::to_string(ip[2]) + "." +
        std::to_string(ip[3]);
}

bool ARPCacheTable::add(const unsigned char* ip, const unsigned char* mac, const bool& state, const bool& isPermanent) {
    std::string strIP = binaryToString(ip);
    auto it = cache.find(strIP);
    if (it != cache.end()) {
        // AfxMessageBox(_T("exist")); 이미존재
        return false;
    }
    else {
        cache[strIP] = new ARPCacheEntry(ip, mac, state, this, isPermanent);
        return true;
    }
}

bool ARPCacheTable::update(const unsigned char* ip, const unsigned char* mac) {
    std::string strIP = binaryToString(ip);
    auto it = cache.find(strIP);
    if (it != cache.end()) {
        editEntryMacAddress(ip, mac);
        return true;
    }
    else {
        return false;
    }
}

void ARPCacheTable::handleArpReply(const unsigned char* ip) {
    std::string strIP = binaryToString(ip);
    auto it = cache.find(strIP);
    if (it != cache.end()) {
        it->second->onArpReplyReceived();
    }
}

bool ARPCacheTable::editEntryMacAddress(const unsigned char* ip, const unsigned char* mac) {
    if (!ip||!mac) false;
    std::string strIP = binaryToString(ip);
    auto it = cache.find(strIP);
    it->second->editMac(mac);
    return true;
}

bool ARPCacheTable::removeEntry(const unsigned char* ip) {
    std::string strIP = binaryToString(ip);
    auto it = cache.find(strIP);
    if (it != cache.end()) {
        delete it->second;
        cache.erase(it);
        return true;
    }
    return false;
}

bool ARPCacheTable::clearAll() {
    if (cache.empty()) {
        return false; // cache가 이미 비어 있는 경우 false 반환
    }
    try {
        for (auto& pair : cache) {
            delete pair.second;  // ARPCacheEntry 객체의 메모리 해제
        }
        cache.clear();  // 맵 초기화
    }
    catch (const std::exception& e) {
        return false;  // 메모리 접근 실패 시 false 반환
    }
    //cache.clear();
    return true;
}

CString ARPCacheTable::printCache() const {
    CString accumulatedMessage; // 모든 메시지를 누적할 CString 변수

    for (const auto& pair : cache) {
        const auto& ip = pair.first;
        const auto& entry = pair.second;

        CString CIp(entry->getIP().c_str());
        CString CMac(entry->getMAC().c_str());
        CString CState(entry->getState().c_str());

        CString line;
        line.Format(_T("IP: %s, MAC: %s, State: %s\r\n"), CIp, CMac, CState); // 각 항목을 한 줄로 포맷

        accumulatedMessage += line; // 누적된 메시지에 추가
    }
    //AfxMessageBox(accumulatedMessage);
    return accumulatedMessage;
    // 루프가 끝난 후 한 번의 메시지 박스로 모든 항목을 표시
    
}

void ARPCacheTable::print() const {
    CString accumulatedMessage; // 모든 메시지를 누적할 CString 변수

    for (const auto& pair : cache) {
        const auto& entry = pair.second;

        if (entry == nullptr) {
            continue;
        }
        CString CIp(entry->getIP().c_str());
        CString CMac(entry->getMAC().c_str());
        CString CState(entry->getState().c_str());

        CString line;
        line.Format(_T("IP: %s, MAC: %s, State: %s\r\n"), CIp, CMac, CState);

        accumulatedMessage += line; // 누적된 메시지에 추가
    }
    AfxMessageBox(accumulatedMessage);
}
