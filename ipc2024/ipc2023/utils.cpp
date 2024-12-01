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
	std::string CS2STR(const CString& cStr) {
		return std::string(cStr.GetString());
	}
	void macCS2B(CString& mac, UCHAR* uchar) {
		const char* cstr = mac.GetString();
		sscanf_s(cstr, "%02x:%02x:%02x:%02x:%02x:%02x",
				&uchar[0], &uchar[1], &uchar[2],
				&uchar[3], &uchar[4], &uchar[5]);
	}
	void ipCS2B(CString& ip, UCHAR* uchar) {
		const char* cstr = ip.GetString();
		sscanf_s(cstr, "%d.%d.%d.%d",
			&uchar[0], &uchar[1], &uchar[2], &uchar[3]);
	}
	void macSTR2B(std::string& mac, UCHAR* uchar) {
		int v[6];
		sscanf_s(mac.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x",
			&v[0], &v[1], &v[2],
			&v[3], &v[4], &v[5]);
		for (int i = 0; i < 6; i++) {
			uchar[i] = static_cast<unsigned char>(v[i]);
		}
	}
	void ipSTR2B(std::string& ip, UCHAR* uchar) {
		int v[4];
		sscanf_s(ip.c_str(), "%d.%d.%d.%d",
			&v[0], &v[1], &v[2], &v[3]);
		for (int i = 0; i < 4; i++) {
			uchar[i] = static_cast<unsigned char>(v[i]);
		}
	}
	CString STR2CS(const std::string& str) {
		return CString(str.c_str());
	}
	std::string B2IP(const unsigned char* binarySeq) {
		return std::to_string(binarySeq[0]) + "." +
			std::to_string(binarySeq[1]) + "." +
			std::to_string(binarySeq[2]) + "." +
			std::to_string(binarySeq[3]);
	}
	std::string B2MAC(const unsigned char* binarySeq) {
		char buffer[18];
		sprintf_s(buffer, "%02X:%02X:%02X:%02X:%02X:%02X",
			binarySeq[0],
			binarySeq[1],
			binarySeq[2],
			binarySeq[3],
			binarySeq[4],
			binarySeq[5]);
		return std::string(buffer);
	}	
}

void masking(const unsigned char ip[4], const unsigned char mask[4], unsigned char network[4]) {
	for (int i = 0; i < 4; ++i) {
		network[i] = ip[i] & mask[i];
	}
}
