#pragma once

#include <cstdint>

class SlottedPage {
 private:
    char* header;
    Slot* firstSlot;
    Slot* firstEmptySlot;
    char* slotEnd;
    char* end;
    char* freeSpace;  
 public:
	SlottedPage();	

	/*
	 * Inserts a record and returns the new offset.
	 */
	uint64_t insert(const Record& r);

	/*
	 * Removes the slot from the page.
	 */
	void remove(uint64_t slot);
		
	/*
	 * Compresses the page and returns the new amount of free space 
	 */
    uint64_t recompress();

    Slot* getSlot(uint64_t slotNum);

	Slot* getFreeSlot();

    Record* getRecordPtr(uint64_t slotNum); 
    // oder Record* getRecordPtr(uint64_t intraPageOffset);

	unsigned getFreeSpaceOnPage();
	
	~SlottedPage();
};
