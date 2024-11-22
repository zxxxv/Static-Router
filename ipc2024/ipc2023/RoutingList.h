#pragma once
#include <list>
#include <optional>
#include <iostream>
#include "utils.h"

constexpr unsigned short MAX_FLAG_VALUE = 3;

using namespace RoutingEntry;

class RoutingList {
private:
	std::list<RoutingEntry::Fields> m_list;
	Fields m_buffEnty;
	/*
	optional 헤더는 c++17 이상에서만 사용 가능.
	프로젝트 -> 속성 -> C/C++ -> 언어 -> C++ Language Standard 에서 변경
	optional을 사용하지 않으면 반환 값을 nullptr로 설정할 수 없음.
	*/
	std::optional<Fields> getNextEntry(std::list<Fields>::iterator currentIt);
public:
	RoutingList();
	void initBuffEntry();
	bool isBuffEntry();
	/*bool setBuffEntry(e_field field, const unsigned char* binarySeq);
	bool setBuffEntry(e_field field, const unsigned short srt);*/
	bool setBuffEntry(const unsigned char* dst, const unsigned char* mask, const unsigned char* gate, e_flag flag, const unsigned short intf, const unsigned short metr);
	bool addEntry();
	bool deleteEntry(int entryIndex);
	bool editEntry(int entryIndex, e_field field, const unsigned char* input);
	bool editEntry(int entryIndex, e_field field, const unsigned short srt);
	void printList();
};