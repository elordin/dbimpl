#include <cerrno>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "SPSegment.hpp"
#include "SlottedPage.hpp"
#include "SlottedPage.cpp"

using namespace std;

SPSegment::SPSegment(BufferManager* bm, uint64_t pageSize)
	: bm(bm),
	  pageSize(pageSize){
}

TID SPSegment::insert(const Record& r){

	// Find page with enough space for r
	bool checked = false;
	BufferFrame frame(0);
	SlottedPage* page_test = nullptr;
	for (unsigned p=0; p<bm->getPageCount(); ++p) { 
		BufferFrame& frame_test = bm->fixPage(p, false);
		page_test = reinterpret_cast<SlottedPage*>(frame_test.getData());
		unsigned freeSpaceOnPage = page_test->getFreeSpaceOnPage();
		//cout << "Free: " << freeSpaceOnPage << ", Length: " << r.getLen() << endl;
    	if (freeSpaceOnPage >= r.getLen()) {
			BufferFrame& frame = frame_test;
			checked = true;
			break;
    	}
		bm->unfixPage(frame_test, false);
    }

	if(!checked){
		perror("There is not enough space to insert the record");
        throw "There is not enough space to insert the record";
	}

    SlottedPage* page = reinterpret_cast<SlottedPage*>(frame.getData());
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
