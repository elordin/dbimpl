#include <cerrno>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "SPSegment.hpp"
#include "SlottedPage.hpp"
#include "SlottedPage.cpp"

using namespace std;

SPSegment::SPSegment(uint64_t segmentId, BufferManager* bm, uint64_t pageSize)
	: bm(bm),
	  pageSize(pageSize),
      lastPage(0),
      segmentId(segmentId) {
}

SlottedPage* SPSegment::addPage() {
    this->lastPage + 1
    BufferFrame frame = bm->fixPage(this->lastPage);

    SlottedPage* page = new SlottedPage();
    memcpy(frame->getData(), page, PAGE_SIZE);

    return frame->getData();
}

SlottedPage* SPSegment::getFreeSpace(unsigned spaceRequired) {
    for (uint64_t i = 0; i < this->lastPage; i++) {
        uint64_t pageId = (this->segmentID << 48) | (i << 6);

        BufferFrame frame = bm->fixPage(pageId, true);
        SlottedPage* sp = reinterpret_cast<SlottedPage*>(frame->getData());
        if (sp->freeSpaceOnPage() > spaceRequired)
            return sp;
        bm->unfixPage(frame, false);
    }
    return this->addPage();
}


TID SPSegment::insert(const Record& r){

	// Find page with enough space for r
    SlottedPage* page = this->getFreeSpace(r.getLen());

    // Reorder record ?

    // Write to page
    uint64_t offset = page->insert(r);

    // Return TID
	Slot* slot = page->getSlot(offset);
	return slot->tid;
}

bool SPSegment::remove(TID tid){
    BufferFrame frame = bm->fixPage(tid.getPage(), true);

    SlottedPage* page = reinterpret_cast<SlottedPage*>(frame.getData());

    page->remove(tid.getSlot());
    page->recompress();

    bm->unfixPage(frame, true);

    return true;
}

Record SPSegment::lookup(TID tid) {
    BufferFrame frame = bm->fixPage(tid.getPage(), true);

    SlottedPage* page = reinterpret_cast<SlottedPage*>(frame.getData());
    Slot* slot = page->getSlot(tid.getSlot());

    if (slot->length == 0 && slot->offset == 0)
        return Record(0, nullptr); // Slot is empty, return empty record
    else
        return Record(slot->length, slot->getRecord()->getData());
}

bool SPSegment::update(TID tid, const Record& r){
	Record r_old = this->lookup(tid);
	unsigned len_old = r_old.getLen();
	unsigned len_new = r.getLen();
	BufferFrame frame = bm->fixPage(tid.getPage(), true);
    SlottedPage* page = reinterpret_cast<SlottedPage*>(frame.getData());
	// If size doesn't change, use memcpy
	if(len_old == len_new){
		memcpy(page->getRecordPtr(tid.getSlot()), &r, r.getLen());
	} else if(len_old > len_new){
		memcpy(page->getRecordPtr(tid.getSlot()), &r, r.getLen());
        page->recompress();
	} else {
		unsigned freeSpaceOnPage = page->getFreeSpaceOnPage();
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

SPSegment::~SPSegment() {
    delete bm;
}
