#pragma once

#include <cstdint>

struct TID {
    uint64_t tid;

    TID(uint64_t tid);
    uint64_t getSlot();
    uint64_t getPage();
    uint64_t getTID();
    ~TID();
}; 
