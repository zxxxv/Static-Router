#pragma once
#include "pch.h"
#include <list>
#include <optional>
#include <iostream>
#include "utils.h"

constexpr unsigned short MAX_FLAG_VALUE = 3;

using namespace RoutingEntry;

class RoutingList {

public:
    static RoutingList& GetInstance() {
        static RoutingList instance;
        return instance;
    }

    void initBuffEntry();
    bool isBuffEntry();
    bool setBuffEntry(const unsigned char* dst, const unsigned char* mask, const unsigned char* gate, e_flag flag, const unsigned short intf, const unsigned short metr);
    bool addEntry();
    bool deleteEntry(int entryIndex);
    bool editEntry(int entryIndex, e_field field, const unsigned char* input);
    bool editEntry(int entryIndex, e_field field, const unsigned short srt);
    void printList();
    std::list<RoutingEntry::Fields> RoutingList::getAllEntries();
    
    Fields findEntry(const unsigned char* dst);
    Fields m_buffEnty;

private:
    RoutingList() { initBuffEntry(); };
    ~RoutingList() = default;
    RoutingList(const RoutingList&) = delete;
    RoutingList& operator=(const RoutingList&) = delete;

    std::list<RoutingEntry::Fields> m_list;
    

    std::list<Fields>::iterator getNextEntry(std::list<Fields>::iterator currentIt);
	bool isMatchingEntry(std::list<Fields>::iterator currentIt, const unsigned char* dst);

};

