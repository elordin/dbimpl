#pragma once

#include <cstdint>

struct TID {
    uint64_t tid;

    TID(uint64_t tid);
    TID(uint64_t pageId, unsigned slot);
    uint64_t getSlot();
    uint64_t getPage();
    ~TID();
};
