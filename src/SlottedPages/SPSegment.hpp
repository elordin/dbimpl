#pragma once

#include <vector>
#include <cstdint>
#include <map>

#include "TID.hpp"
#include "Record.hpp"
#include "../BufferManager/BufferManager.hpp"
#include "../BufferManager/BufferFrame.hpp"
#include "../parser/Schema.hpp"
#include "../Operators/Register.hpp"
#include "SlottedPage.hpp"


class SPSegment {
 private:
	BufferManager* bm;
    TID lastTID;
    uint64_t lastPage;
    uint64_t segmentId;
    std::map<uint64_t, unsigned> fsi;
 public:
    /**
     *  Constructor for a new SPSegment
     */
    SPSegment(uint64_t segmentId, BufferManager* bm);

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

    unsigned size();

    std::vector<Register*> toRegisterVector(const char* data);

    Schema::Relation relation();

    /**
     *  Deconstructor
     */

    ~SPSegment();
};
