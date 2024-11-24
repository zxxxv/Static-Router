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
    static RoutingList& getInstance() {
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

private:
    RoutingList() { initBuffEntry(); };
    ~RoutingList() = default;
    RoutingList(const RoutingList&) = delete;
    RoutingList& operator=(const RoutingList&) = delete;

    std::list<RoutingEntry::Fields> m_list;
    Fields m_buffEnty;

    std::optional<Fields> getNextEntry(std::list<Fields>::iterator currentIt);

};
