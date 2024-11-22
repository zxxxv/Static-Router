#include "routingList.h"

RoutingList::RoutingList() {
	initBuffEntry();
}

void RoutingList::initBuffEntry() {
	std::memset(m_buffEnty.m_destination, 0, 4);
	std::memset(m_buffEnty.m_subnetMask, 0, 4);
	std::memset(m_buffEnty.m_gateway, 0, 4);
	m_buffEnty.m_flag = e_flag::none;
	m_buffEnty.m_interfaceFlag = 0;
}

bool RoutingList::isBuffEntry() {
	if (std::memcmp(m_buffEnty.m_destination, "\0\0\0\0", 4) == 0) return false;
	if (std::memcmp(m_buffEnty.m_subnetMask, "\0\0\0\0", 4) == 0) return false;
	if (std::memcmp(m_buffEnty.m_gateway, "\0\0\0\0", 4) == 0) return false;
	if (m_buffEnty.m_flag == e_flag::none) return false;
	if (m_buffEnty.m_interfaceFlag == 0) return false;

	return true;
}

//bool RoutingList::setBuffEntry(e_field field, const unsigned char* binarySeq) {
//	if (binarySeq == nullptr) return false;
//
//	switch (field) {
//	case e_field::dst:
//		memcpy(m_buffEnty.m_destination, binarySeq, 4);
//		break;
//	case e_field::mask:
//		memcpy(m_buffEnty.m_subnetMask, binarySeq, 4);
//		break;
//	case e_field::gate:
//		memcpy(m_buffEnty.m_gateway, binarySeq, 4);
//		break;
//	default:
//		return false;
//	}
//	return true;
//};
//
//bool RoutingList::setBuffEntry(e_field field, const unsigned short srt) {
//	switch (field) {
//	case e_field::flag:
//		if (srt > MAX_FLAG_VALUE) return false;
//		m_buffEnty.m_flag = static_cast<e_flag> (srt);
//		break;
//	case e_field::intf:
//		m_buffEnty.m_interfaceFlag = srt;
//		break;
//	default:
//		return false;
//	}
//	return true;
//};

bool RoutingList::setBuffEntry(const unsigned char* dst, const unsigned char* mask, const unsigned char* gate, e_flag flag, const unsigned short intf) {
	if (dst == nullptr) return false;
	if (mask == nullptr) return false;
	if (gate == nullptr) return false;
	memcpy(m_buffEnty.m_destination, dst, 4);
	memcpy(m_buffEnty.m_subnetMask, mask, 4);
	memcpy(m_buffEnty.m_gateway, gate, 4);
	m_buffEnty.m_flag = flag;
	m_buffEnty.m_interfaceFlag = intf;

	return true;
}

bool RoutingList::addEntry() {
	if (isBuffEntry() == false) return false;
	m_list.push_back(m_buffEnty);
	initBuffEntry();
	return true;
};

std::optional<Fields> RoutingList::getNextEntry(std::list<Fields>::iterator currentIt) {
	auto nextIt = std::next(currentIt);
	if (nextIt != m_list.end()) return *nextIt; // 다음 요소 반환
	else return std::nullopt; // 리스트의 끝일 경우
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
};

void RoutingList::printList() {
	for (const auto& it : m_list) {
		printf("dst: %s | ", Converter::B2IP(it.m_destination).c_str());
		printf("mask: %s | ", Converter::B2IP(it.m_subnetMask).c_str());
		printf("gate: %s | ", Converter::B2IP(it.m_gateway).c_str());
		printf("flag: %s | ", fieldToString(it.m_flag).c_str());
		printf("intf: %d \n", it.m_interfaceFlag);
	}
};