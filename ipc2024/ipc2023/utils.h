#pragma once
#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <string>
#include <cstdio>

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
	};
}

namespace Converter {
	bool CS2B();
	bool IP2B();
	bool MAC2B();
	bool B2CS(const unsigned char* input);
	std::string B2IP(const unsigned char* input);
	bool B2MAC(const unsigned char* input);
}

#endif 