// ARPCacheTable.h
#ifndef ARPCACHETABLE_H
#define ARPCACHETABLE_H

#include "pch.h"
#include <unordered_map>
#include <iostream>
#include <string>
#include "ARPCacheEntry.h"

// ARPCacheTable 클래스 정의
class ARPCacheTable : public TimeoutObserver {
private:
    std::unordered_map<std::string, ARPCacheEntry*> cache;

public:
    // 기본 생성자 추가
    ARPCacheTable();
    ~ARPCacheTable();

    std::string binaryToString(const unsigned char* ip);
    /*
    binaryToString: binary ip를 10진수 기반 문자열로 변환하는 함수. IP를 해시 테이블의 Key로 사용하기 때문에 필요합니다.
    */

    //void onEntryTimeout(const unsigned char* ip) override;
    /*
    onEntryTimeout: 타임아웃된 항목을 삭제하는 함수. Entry가 타임아웃되면 트리거된다.
        # parameter
        - binary ip 주소, 각 entry의 OnTimer 함수 내에서 입력된다.
    */

    bool add(const unsigned char* ip, const unsigned char* mac, const bool& state, const bool& isPermanent);
    bool update(const unsigned char* ip, const unsigned char* mac);
    /*
    addOrUpdate: entry를 추가하거나, 해당 ip가 이미 테이블에 존재하는지 확인하는 함수.
        # parameter
        - ip: binary ip 주소
        - mac: binary mac 주소
        - state: entry의 상태 값
            - ture: conplete
            - false: incomplete
        - isPermanent: entry 타이머의 시간을 설정하는 bool 값
            - true: 20분 타이머
            - false: 3분 타이머
        # return value
        - true: 입력된 ip가 테이블에 존재하지 않아 새로운 entry를 생성하고 테이블에 추가했음
        - false: 입력된 ip가 테이블에 존재하여 entry를 생성하지 않고 바로 반환한 상태
    */

    void handleArpReply(const unsigned char* ip);
    /*
    handleArpReply: ARP 응답 패킷(OP Code == 2)일 때 아래의 작업을 수행하는 함수
    - state 변경: incomplete -> complete
    - timer 변경: 3분 타이머 제거, 20분 타이머 설정
    */

    bool removeEntry(const unsigned char* ip);
    /*
    removeEntry: 입력된 ip 주소를 기반으로 해당되는 entry를 테이블에서 제거하는 함수
    */

    bool editEntryMacAddress(const unsigned char* ip, const unsigned char* mac);
    /*
    editEntry: 엔트리의 mac 주소를 바꾸는 함수
        # parameter
        - ip: 엔트리를 식별하기 위한 ip 주소
        - mac: 바꾸고자하는 mac 주소
        # return value
        - true
        - false
    */

    bool clearAll();

    // 캐시 출력 (디버깅용)
    CString printCache() const;
    void print() const;
};

#endif // ARPCACHETABLE_H
