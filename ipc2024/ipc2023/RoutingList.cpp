#include "stdafx.h"
#include "pch.h"
#include "routingList.h"

void RoutingList::initBuffEntry() {
	m_buffEnty.m_flag = e_flag::none;
}

bool RoutingList::isBuffEntry() {
	if (m_buffEnty.m_flag == e_flag::none) {
		AfxMessageBox("Error: Flag 설정 오류");
		return false;
	}
	//AfxMessageBox("유효한 Buffer entry");
	return true;
}

bool RoutingList::setBuffEntry(const unsigned char* dst, const unsigned char* mask, const unsigned char* gate, e_flag flag, const unsigned short intf, const unsigned short metr) {
	if (dst == nullptr) return false;
	if (mask == nullptr) return false;
	if (gate == nullptr) return false;
	memcpy(m_buffEnty.m_destination, dst, 4);
	memcpy(m_buffEnty.m_subnetMask, mask, 4);
	memcpy(m_buffEnty.m_gateway, gate, 4);
	m_buffEnty.m_flag = flag;
	m_buffEnty.m_interfaceFlag = intf;
	m_buffEnty.m_metric = metr;

	return true;
}

bool RoutingList::addEntry() {
	if (isBuffEntry() == false) return false;
	m_list.push_back(m_buffEnty);
	initBuffEntry();
	return true;
};

std::list<Fields>::iterator RoutingList::getNextEntry(std::list<Fields>::iterator currentIt) {
	return std::next(currentIt);
}

bool RoutingList::isMatchingEntry(std::list<Fields>::iterator currentIt, const unsigned char* dst)
{	
	if (!dst) return false;

	unsigned char network[4];
	masking(dst, currentIt->m_subnetMask, network);

	if (memcmp(currentIt->m_destination, network, 4) == 0) return true;
	// memcmp는 같을 때 0을 리턴

	return false;
}

std::list<RoutingEntry::Fields> RoutingList::getAllEntries() {
    return m_list;
}

bool RoutingList::deleteEntry(int entryIndex) {
	if (entryIndex >= m_list.size()) return false; // 유효하지 않은 인덱스일 경우
	auto it = m_list.begin(); // 리스트의 시작 반복자
	std::advance(it, entryIndex);  // index 만큼 이동
	m_list.erase(it);         // 해당 요소 제거
	return true;
};

bool RoutingList::editEntry(int entryIndex, e_field field, const unsigned char* binarySeq) {
	if (entryIndex >= m_list.size()) return false; // 유효하지 않은 인덱스일 경우
	auto it = m_list.begin(); // 리스트의 시작 반복자
	std::advance(it, entryIndex);  // index 만큼 이동

	if (binarySeq == nullptr) return false;

	switch (field) {
	case e_field::dst:
		memcpy(it->m_destination, binarySeq, 4);
		break;
	case e_field::mask:
		memcpy(it->m_subnetMask, binarySeq, 4);
		break;
	case e_field::gate:
		memcpy(it->m_gateway, binarySeq, 4);
		break;
	default:
		return false;
	}

	return true;
};

bool RoutingList::editEntry(int entryIndex, e_field field, const unsigned short srt) {
	if (entryIndex >= m_list.size()) return false; // 유효하지 않은 인덱스일 경우
	auto it = m_list.begin(); // 리스트의 시작 반복자
	std::advance(it, entryIndex);  // index 만큼 이동

	switch (field) {
	case e_field::flag:
		if (srt > MAX_FLAG_VALUE) return false;
		it->m_flag = static_cast<e_flag> (srt);
		break;
	case e_field::intf:
		it->m_interfaceFlag = srt;
		break;
	default:
		return false;
	}

	return true;
}

Fields RoutingList::findEntry(const unsigned char* dst)
{
	// 리스트의 처음부터 탐색 시작
	std::list<Fields>::iterator it = m_list.begin();

	while (it != m_list.end()) {
		// 현재 엔트리가 목적지 주소와 일치하는지 확인
		if (isMatchingEntry(it, dst)) {
			// 일치하는 엔트리를 찾은 경우 현재 엔트리를 반환
			return *it;
		}
		else {
			// getNextEntry는 현재 엔트리에서 다음 엔트리를 반환하는 함수
			// optional 반환이므로 값이 존재하는지 확인 후 언랩
			it = getNextEntry(it);
		}
	}
	// 일치하는 엔트리가 없는 경우, 기본 엔트리 반환
	return m_buffEnty;
}

//Fields RoutingList::findEntry(const unsigned char* dst)
//{
//	std::list<Fields>::iterator it = m_list.begin();
//	// 맞는 엔트리 위치 찾기
//	while (it != m_list.end()) {
//		if (isMatchingEntry(it, dst)) {
//			// ip가 현재 엔트리와 매칭된 경우. iterator가 현재 엔트리를 가리킨다.
//			break;
//		}
//		else {
//			auto nextIt = getNextEntry(it);
//			// 현재 엔트리에서 탐색 실패 시 다음 엔트리로 이동.
//			// getNextEntry는 다음 엔트리가 존재할 때, NextIt를 반환 / 없을 때, nullptr을 반환한다.
//			if (!nextIt) return m_buffEnty;
//			// nextIt가 nullptr이면(다음 엔트리가 존재하지 않을 때) m_buffEnty를 반환한다.(e_flag가 none인 entry)
//			else it = *nextIt;
//			// nextIt가 nullptr이 아니면, 다음 엔트리가 존재한다는 의미이며, NextIt에 대해 while문을 반복한다.
//		}
//	}
//
//	// 현재 엔트리를 반환한다.
//	return *it;
//}

void RoutingList::printList() {
	for (const auto& it : m_list) {
		printf("dst: %s | ", Converter::B2IP(it.m_destination).c_str());
		printf("mask: %s | ", Converter::B2IP(it.m_subnetMask).c_str());
		printf("gate: %s | ", Converter::B2IP(it.m_gateway).c_str());
		printf("flag: %s | ", fieldToString(it.m_flag).c_str());
		printf("intf: %d \n", it.m_interfaceFlag);
	}
};
