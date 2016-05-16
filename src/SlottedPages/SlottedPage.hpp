#pragma once

#include <cstdint>
#include "SPSegment.hpp"
#include "Record.hpp"


struct Slot {
    TID      tid;
    uint64_t offset;
    uint64_t length;
    bool     moved;
    Slot() : offset(0), length(0), tid(TID(0)), moved(false) {}
    bool isEmpty() { return this->offset == 0 && this->length == 0; }
    Record* getRecord() { return reinterpret_cast<Record*>(this + offset); }
};


class SlottedPage {
 private:
    char* header;
    Slot* firstSlot;
    Slot* firstEmptySlot;
    Slot* slotEnd;
    char* end;
    char* freeSpace;
	uint64_t pageId;
 public:
	SlottedPage(uint64_t pageId);

	/*
	 * Inserts a record and returns the TID just inserted.
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
