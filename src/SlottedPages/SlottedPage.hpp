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
		
	/** Compresses the page, returns the new amount of free space */
    uint64_t recompress();

    Slot* getSlot(uint64_t slotNum);

    Record* getRecordPtr(uint64_t slotNum); 
    // oder Record* getRecordPtr(uint64_t intraPageOffset);
	
	~SlottedPage();
}
