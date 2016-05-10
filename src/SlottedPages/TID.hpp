#pragma once

#include <cstdint>

class TID {
 private:
	uint64_t pageID;
	uint64_t slotID;

 public:
    /**
     *  Constructor for a new TID
     */
    TID(uint64_t pageID, uint64_t slotID);

	uint64_t getPage();
	
	uint64_t getSlot();

    /**
     *  Deconstructor
     */

    ~TID();
};
