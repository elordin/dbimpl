#include <cerrno>
#include <cstdio>
#include <iostream>
#include <cstdlib>

#include "SPSegment.hpp"
#include "SlottedPage.hpp"
#include "SlottedPage.cpp"

using namespace std;


SPSegment::SPSegment(uint64_t segmentId, BufferManager* bm)
	: bm(bm),
      lastPage(0),
      segmentId(segmentId),
      lastTID(0),
      fsi(std::map<uint64_t, unsigned>()) {}


TID SPSegment::insert(const Record& r){

	// Find page with enough space for r
    uint64_t pageId = this->lastPage + 1;

    for (auto it = this->fsi.rbegin(); it != this->fsi.rend(); it++) {
        if (it->second >= r.getLen()) {
            pageId = it->first;
            break;
        }
    }

    // If necessary, create new SlottedPage
    BufferFrame frame = bm->fixPage(pageId, true);
    SlottedPage* page = reinterpret_cast<SlottedPage*>(frame.getData());
    if (pageId > this->lastPage) {
        *page = SlottedPage();
        this->lastPage++;
    }

    // TODO Reorder record ?

    // Write to page
    unsigned slotNum = page->insert(r);

    bm->unfixPage(frame, true);
    return TID(pageId, slotNum);
}

bool SPSegment::remove(TID tid){

    // TODO assert(tid.getPage() is part of this segment);

    BufferFrame frame = bm->fixPage(tid.getPage(), true);
    SlottedPage* page = reinterpret_cast<SlottedPage*>(frame.getData());

    unsigned space = page->remove(tid.getSlot());

    // Update FSI
    this->fsi[tid.getPage()] = space;

    bm->unfixPage(frame, true);
    return true;
}

Record SPSegment::lookup(TID tid) {
    // TODO assert(tid.getPage() is part of this segment);

    BufferFrame frame = bm->fixPage(tid.getPage(), false);

    SlottedPage* page = reinterpret_cast<SlottedPage*>(frame.getData());

    Slot* slot = page->getSlot(tid.getSlot());

    if (slot->length() == 0 && slot->offset() == 0) {
        bm->unfixPage(frame, false);
        return Record(0, nullptr); // Slot is empty, return empty record
    } else {
        bm->unfixPage(frame, false);
        return Record(slot->length(), slot->getRecord()->getData());
    }
}

bool SPSegment::update(TID tid, const Record& r){

	Record r_old = this->lookup(tid);
	unsigned len_old = r_old.getLen();
	unsigned len_new = r.getLen();

	BufferFrame frame = bm->fixPage(tid.getPage(), true);
    SlottedPage* page = reinterpret_cast<SlottedPage*>(frame.getData());
	// If size doesn't change, use memcpy
	if(len_old == len_new){
		memcpy(page->getSlot(tid.getSlot())->getRecord(), &r, r.getLen());
	} else if(len_old > len_new){
		memcpy(page->getSlot(tid.getSlot())->getRecord(), &r, r.getLen());
        page->recompress();
	} else {
		unsigned freeSpaceOnPage = page->freeSpace();
    	if (freeSpaceOnPage >= len_new) {
			page->remove(tid.getSlot());
			page->recompress();
			uint64_t offset = page->insert(r);
    	} else {
			page->remove(tid.getSlot());
    		page->recompress();
			TID new_tid = insert(r);
            // TODO: indirection to new position
		}
	}
	bm->unfixPage(frame, true);
	return true;
}


std::vector<Register*> SPSegment::toRegisterVector(const char* data) {

}

Schema::Relation SPSegment::relation() {}


SPSegment::~SPSegment() {}
