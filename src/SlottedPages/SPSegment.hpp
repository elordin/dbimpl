#pragma once

#include <cstdint>
#include "TID.hpp"
#include "Record.hpp"
#include "../BufferManager/BufferManager.hpp"
#include "../BufferManager/BufferFrame.hpp"

struct Slot {
    TID      tid;
    uint64_t offset;
    uint64_t length;
    bool     moved;
    Slot() : offset(0), length(0), tid(TID(0)), moved(false) {}
    bool isEmpty() { return this->offset == 0 && this->length == 0; }
    Record* getRecord() { return reinterpret_cast<Record*>(this + offset); }
};


class SPSegment {
 private:
	BufferManager* bm;
	uint64_t pageSize;
    uint64_t lastPage;
    uint64_t segmentId;
 public:
    /**
     *  Constructor for a new SPSegment
     */
    SPSegment(uint64_t segmentId, BufferManager* bm, uint64_t pageSize);

    /**
     *  Loads another fresh page
     */
    SlottedPage* addPage();

    /**
     *  Returns the first page with at least spaceRequired bytes of free space.
     */
    SlottedPage* getFreeSpace(unsigned spaceRequired);



    /**
     *  Provides an interface to insert "records". Searches through the segment’s pages looking for a page with enough space to store r.
	 *  Returns the TID identifying the location where r was stored.
     */
    TID insert(const Record& r);

    /**
     *  Deletes the record pointed to by tid and updates the page header accordingly.
     */
    bool remove(TID tid);

	/**
     *  Returns the read-only record (Record.hpp) associated with TID tid.
     */
	Record lookup(TID tid);

	/**
     *  Updates the record pointed to by tid with the content of record r.
     */
	bool update(TID tid, const Record& r);

    /**
     *  Deconstructor
     */

    ~SPSegment();
};
