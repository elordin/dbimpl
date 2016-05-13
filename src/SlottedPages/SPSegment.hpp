#pragma once

#include <cstdint>
#include "TID.hpp"
#include "Record.hpp"

struct Slot {
    uint64_t offset;
    uint64_t length;
    uint64_t tid;
    bool     moved;
};


class SPSegment {
 private:

 public:
    /**
     *  Constructor for a new SPSegment
     */
    SPSegment();

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
