// ARPCacheEntry.cpp
#include "pch.h"
#include "ARPCacheEntry.h"

UINT_PTR ARPCacheEntry::s_nextTimerId = 1;

ARPCacheEntry::ARPCacheEntry(const unsigned char* ipAddress, const unsigned char* macAddress, const bool& entryState, TimeoutObserver* obs, const bool& isPermanent)
    : m_observer(obs), m_timerId(0) {
    setEntry(ipAddress, macAddress, entryState);
    BOOL createResult = CreateEx(0,                // 확장 스타일 
        AfxRegisterWndClass(0), // 윈도우 클래스 등록 
        _T("ARP Entry"),        // 윈도우 이름
        WS_OVERLAPPED,          // 윈도우 스타일 (겹쳐진 윈도우, 보이지 않음)
        0, 0, 0, 0,             // 위치와 크기 (0으로 설정)
        NULL,                   // 부모 윈도우 없음
        0);
    if (createResult) {
        //AfxMessageBox(_T("윈도우 생성 성공"));
    }
    else {
        //AfxMessageBox(_T("윈도우 생성 실패"));
        return;
    }

    m_timerId = s_nextTimerId++;

    if (isPermanent) {
        // MFC 20분 타이머 설정
        this->SetTimer(m_timerId, 20 * 60 * 1000, nullptr); // 20분(1200초) 타이머 설정, ID 자동 생성
    }
    else {
        // MFC 3분 타이머 설정 (임시 엔트리 생성 시)
        this->SetTimer(m_timerId, 3 * 60 * 1000, nullptr); // 3분(180초) 타이머 설정, ID 자동 생성
    }
}

ARPCacheEntry::~ARPCacheEntry() {
    if (m_timerId != 0) {
        KillTimer(m_timerId); // 타이머 해제
        m_timerId = 0;
    }
    if (m_hWnd != NULL && ::IsWindow(m_hWnd)) {
        DestroyWindow(); // 윈도우 해제
    }
}

void ARPCacheEntry::setEntry(const unsigned char* ipAddress, const unsigned char* macAddress, const bool& entryState) {
    memcpy(m_ip, ipAddress, 4);
    memcpy(m_mac, macAddress, 6);
    m_state = entryState;
}

void ARPCacheEntry::OnTimer(UINT_PTR nIDEvent) {
    // 타이머 만료 시, 엔트리가 응답을 받지 않으면 제거
    if (nIDEvent == m_timerId) {
        if (m_hWnd != NULL && ::IsWindow(m_hWnd)) {
            if (m_observer && m_state == false) {
                // incomplete 상태
                KillTimer(m_timerId); // 3분 타이머 해제
                m_observer->onEntryTimeout(m_ip); // 타임아웃 시 옵저버에게 알림
                // 주의!) onEntryTimeout에서 엔트리 자체를 삭제하기 때문에 m_timerId도 없어집니다. 때문에 항상 killTimer의 뒤에 위치해야 합니다.
                m_timerId = 0;
            }
            else if (m_observer) {
                // complete 상태
                KillTimer(m_timerId); // 타이머 해제
                m_observer->onEntryTimeout(m_ip); // 타임아웃 시 옵저버에게 알림
                m_timerId = 0;
            }
        }
    }
    /*if (m_hWnd != NULL && ::IsWindow(m_hWnd)) {
        if (!KillTimer(nIDEvent)) {
            DWORD error = GetLastError();
            CString errorMessage;
            errorMessage.Format(_T("KillTimer failed with error: %u"), error);
            AfxMessageBox(errorMessage);
        }
        else {
            m_observer->onEntryTimeout(m_ip);
        }
    }
    else {
            AfxMessageBox(_T("Invalid window handle. Cannot kill timer."));
    }*/
}

void ARPCacheEntry::onArpReplyReceived() {
    if (m_timerId != 0) {
        KillTimer(m_timerId); // 기존 타이머 해제 (3분 타이머)
    }
    // 상태를 complete로 변경하고 20분 타이머 설정
    m_state = true;
    SetTimer(m_timerId, 20 * 60 * 1000, nullptr); // 20분(1200초) 타이머 설정, ID 자동 생성
}

bool ARPCacheEntry::editMac(const unsigned char* mac) {
    if (!mac) return false;
    memcpy(m_mac, mac, 6);
    return true;
}

std::string ARPCacheEntry::getIP() {
    return std::to_string(m_ip[0]) + "." +
        std::to_string(m_ip[1]) + "." +
        std::to_string(m_ip[2]) + "." +
        std::to_string(m_ip[3]);
}
std::string ARPCacheEntry::getMAC() {
    char buf[18]; // MAC 주소 형식 (XX:XX:XX:XX:XX:XX) + null 종결자
    sprintf_s(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
        static_cast<unsigned char>(m_mac[0]),
        static_cast<unsigned char>(m_mac[1]),
        static_cast<unsigned char>(m_mac[2]),
        static_cast<unsigned char>(m_mac[3]),
        static_cast<unsigned char>(m_mac[4]),
        static_cast<unsigned char>(m_mac[5]));

    return std::string(buf);
}
std::string ARPCacheEntry::getState() {
    if (m_state == true) return "complete";
    else return "incomplete";
}

BEGIN_MESSAGE_MAP(ARPCacheEntry, CWnd)
    ON_WM_TIMER()
END_MESSAGE_MAP()
