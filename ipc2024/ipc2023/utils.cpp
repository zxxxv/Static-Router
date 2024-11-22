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
		return std::to_string(binarySeq[0]) + "." +
			std::to_string(binarySeq[1]) + "." +
			std::to_string(binarySeq[2]) + "." +
			std::to_string(binarySeq[3]);
	}
	std::string B2MAC(const unsigned char* binarySeq) {
		char buffer[6];
		std::sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X",
			binarySeq[0],
			binarySeq[1],
			binarySeq[2],
			binarySeq[3],
			binarySeq[4],
			binarySeq[5]);
		return std::string(buffer);
	}	
}