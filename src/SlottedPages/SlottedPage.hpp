#pragma once

#include <cstdint>
#include <cassert>

#include "SPSegment.hpp"
#include "Record.hpp"
#include "TID.hpp"



class SlottedPage {
    struct Slot {
        uint64_t slot;

        Slot() {
            Slot(0, 0, false);
        }

        Slot(uint64_t offset, uint64_t length, bool moved) {
            assert(offset < 1 << 24);
            assert(length < 1 << 24);
            this->slot = (((((0xFFl << 8) & moved) << 24) & offset) << 24) & length;
        }

        Slot(TID otherSlot) : slot(otherSlot.tid) {};
        uint64_t length()    { return this->slot & 0xFFFFFFl;                       }
        uint64_t offset()    { return this->slot >> 24 & 0xFFFFFFl;                 }
        bool     isEmpty()   { return this->offset() == 0 && this->length() == 0;   }
        bool     isHere()    { return this->slot >= 0xFF00000000000000l;            }
        bool     isMoved()   { return this->slot & 0x00FF000000000000l;             }
        Record*  getRecord() { return reinterpret_cast<Record*>(this + offset());   }
    };

    struct Header {
        Slot* firstFreeSlot;
        char* dataStart;
        unsigned LSN;
        unsigned slotCount;
        unsigned freeSpace;

        Header();
    };

  private:
    Header* header;
  public:
	SlottedPage();

	/*
	 * Inserts a record and returns the TID just inserted.
	 */
	unsigned insert(const Record& r);

	/*
	 * Removes the slot from the page.
	 */
	void remove(TID tid);




    uint64_t recompress();

    Slot* getSlot(TID tid);

/*
    Slot* getFreeSlot();

    Record* getRecordPtr(uint64_t slotNum);
    // oder Record* getRecordPtr(uint64_t intraPageOffset);

	unsigned getFreeSpaceOnPage();
*/

	~SlottedPage();
};
