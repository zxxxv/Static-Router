#include "pch.h"
#include "stdafx.h"
#include "utils.h"

namespace RoutingEntry {
	std::string fieldToString(e_flag flag) {
		switch (flag) {
		case e_flag::none:    return "none";
		case e_flag::up:      return "up";
		case e_flag::gateway: return "gateway";
		case e_flag::host:    return "host";
		default:              return "unknown";
		}
	}
}

namespace Converter {
	bool CS2B();
	bool IP2B();
	bool MAC2B();
	bool B2CS(const unsigned char* input);
	std::string B2IP(const unsigned char* binarySeq) {
		char buffer[16]; // IP 주소 형식은 최대 "255.255.255.255"로 15자 + null terminator
		sprintf_s(buffer, "%d.%d.%d.%d",
			static_cast<int>(binarySeq[0]),
			static_cast<int>(binarySeq[1]),
			static_cast<int>(binarySeq[2]),
			static_cast<int>(binarySeq[3]));
		return std::string(buffer);
	}
	bool B2MAC(const unsigned char* input);
}