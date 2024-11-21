#pragma once
// TimeoutObserver.h
#include <string>
#include "pch.h"

// TimeoutObserver 인터페이스 정의
class TimeoutObserver {
public:
    virtual void onEntryTimeout(const unsigned char* ip) = 0;
    // 타임아웃 시 호출될 메서드
    // 각 클래스에서 오버라이드하여 사용
};
