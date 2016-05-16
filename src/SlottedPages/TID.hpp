#pragma once

#include <cstdint>

class TID {
 private:
    uint64_t tid;

 public:
    /**
     *  Constructor for a new TID
     */
    TID(uint64_t tid);

    uint64_t getPage();

    uint64_t getSlot();

	uint64_t getTID();

    /**
     *  Deconstructor
     */
    ~TID();
};
