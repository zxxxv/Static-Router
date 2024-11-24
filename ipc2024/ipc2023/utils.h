#pragma once
#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <string>
#include <cstdio>
#include <afxstr.h>

namespace RoutingEntry {
	enum class e_flag {
		none,
		up,
		gateway,
		host
	};

	enum class e_field {
		dst,
		mask,
		gate,
		flag,
		intf
	};

	std::string fieldToString(e_flag flag);

	struct Fields {
		unsigned char m_destination[4];
		unsigned char m_subnetMask[4];
		unsigned char m_gateway[4];
		e_flag m_flag;
		unsigned short m_interfaceFlag;
		unsigned short m_metric;
	};
}

namespace Converter {
	std::string CS2STR(const CString& Cstr);
	void CS2B(CString& mac, UCHAR* uchar);
	void CS2B(CString& ip, UCHAR* uchar);
	void STR2B(std::string& mac, UCHAR* uchar);
	void STR2B(std::string& ip, UCHAR* uchar);
	CString STR2CS(const std::string& string);
	std::string B2IP(const unsigned char* binarySeq);
	std::string B2MAC(const unsigned char* binarySeq);
}

void masking(const unsigned char ip[4], const unsigned char mask[4], unsigned char network[4]);

#endif 