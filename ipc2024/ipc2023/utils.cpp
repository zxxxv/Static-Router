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
	CString B2CS_IP(const unsigned char* binarySeq)
	{
		CString result;
		result.Format(_T("%d.%d.%d.%d"), binarySeq[0], binarySeq[1], binarySeq[2], binarySeq[3]);
		return result;
	}
	CString B2CS_MAC(const unsigned char* binarySeq)
	{
		CString result;
		result.Format(_T("%02x:%02x:%02x:%02x:%02x:%02x"), 
			binarySeq[0], binarySeq[1], binarySeq[2], 
			binarySeq[3], binarySeq[4], binarySeq[5]);
		return result;
	}
	std::string CS2STR(const CString& cStr) {
		return std::string(cStr.GetString());
	}
	void CS2B(CString& mac, UCHAR* uchar) {
		const char* cstr = mac.GetString();
		sscanf_s(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
				&uchar[0], &uchar[1], &uchar[2],
				&uchar[3], &uchar[4], &uchar[5]);
	}
	void CS2B(CString& ip, UCHAR* uchar) {
		const char* cstr = ip.GetString();
		sscanf_s(ip, "%d.%d.%d.%d",
			&uchar[0], &uchar[1], &uchar[2], &uchar[3]);
	}
	void STR2B(std::string& mac, UCHAR* uchar) {
		sscanf_s(mac.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x",
			&uchar[0], &uchar[1], &uchar[2],
			&uchar[3], &uchar[4], &uchar[5]);
	}
	void STR2B(std::string& ip, UCHAR* uchar) {
		sscanf_s(ip.c_str(), "%d.%d.%d.%d",
			&uchar[0], &uchar[1], &uchar[2], &uchar[3]);
	}
	CString STR2CS(const std::string& str) {
		return CString(str.c_str());
	}
	std::string B2STR_IP(const unsigned char* binarySeq) {
		return std::to_string(binarySeq[0]) + "." +
			std::to_string(binarySeq[1]) + "." +
			std::to_string(binarySeq[2]) + "." +
			std::to_string(binarySeq[3]);
	}
	std::string B2STR_MAC(const unsigned char* binarySeq) {
		char buffer[6];
		sprintf_s(buffer, "%02X:%02X:%02X:%02X:%02X:%02X",
			binarySeq[0], binarySeq[1], binarySeq[2],
			binarySeq[3], binarySeq[4], binarySeq[5]);
		return std::string(buffer);
	}	
}

void masking(const unsigned char ip[4], const unsigned char mask[4], unsigned char network[4]) {
	for (int i = 0; i < 4; ++i) {
		network[i] = ip[i] & mask[i];
	}
}
