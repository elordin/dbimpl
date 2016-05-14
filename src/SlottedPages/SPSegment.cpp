#include <cerrno>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "SPSegment.hpp"
#include "SlottedPage.hpp"
#include "../BufferManager/BufferManager.hpp"
#include "../BufferManager/BufferFrame.hpp"

using namespace std;

SPSegment::SPSegment(uint64_t pageSize)
	: bm(new BufferManager(100)),
	  pageSize(pageSize){
}

TID SPSegment::insert(const Record& r){

	// Find page with enough space for r
	bool checked = false; 
	for (unsigned p=0; p<100; ++p) {
		BufferFrame frame = bm->fixPage(p, false);
		SlottedPage* page = reinterpret_cast<SlottedPage*> frame.getData();
		unsigned freeSpaceOnPage = page->getFreeSpaceOnPage();
    	if (freeSpaceOnPage >= r.getLen()) {
			// Do we have to unfix the frame before?
			BufferFrame frame = bm->fixPage(p, true);
			checked = true;
			break;
    	}
    }	
	if(!checked){
		perror("There is not enough space to insert the record");
        throw "There is not enough space to insert the record";
	}

    SlottedPage* page = reinterpret_cast<SlottedPage*> frame.getData();
    // Reorder record ?

    // Write to page
    uint64_t offset = page->insert(r);
    Slot* slot = page->getFreeSlot();
    slot->length = r.getLen();
    slot->offset = offset;

    // Return TID
}

bool SPSegment::remove(TID tid){
    BufferFrame frame = bm->fixPage(tid.getPage(), true);
    SlottedPage* page = reinterpret_cast<SlottedPage*> frame.getData();
    page->remove(tid.getSlot());
    page->recompress();
}

Record SPSegment::lookup(TID tid) {
    BufferFrame frame = bm->fixPage(tid.getPage(), true);
    SlottedPage* page = reinterpret_cast<SlottedPage*> frame.getData();
    Slot* slot = page->getSlot(tid.getSlot());

    if (slot->length == 0 && slot->offset == 0)
        return Record(0, nullptr); // Slot is empty, return empty record
    else
        return Record(slot->length, page->getRecordPtr(slot->offset));
}

bool SPSegment::update(TID tid, const Record& r){
    
	Record r_old = this->lookup(tid);
	unsigned len_old = r_old.getLen();
	unsigned len_new = r.getLen();
	// If size doesn't change
	if(len_old == len_new){
		overWriteData(tid, tid.getSlot(), r);
	} else if(len_old > len_new){
        overWriteData(tid, tid.getSlot(), r);
        // [Optional] Re-compress
	} else {
		BufferFrame frame = bm->fixPage(tid.getPage(), true);
		SlottedPage* page = reinterpret_cast<SlottedPage*> frame.getData();
		unsigned freeSpaceOnPage = page->getFreeSpaceOnPage();
    	if (freeSpaceOnPage >= len_new) {
			overWriteData(tid, tid.getSlot(), r);
    	} else {
			remove(tid);			
			TID new_tid = insert(r);
            // TODO: indirection to new position
		}
	}
}

void SPSegment::overWriteData(TID tid, uint64_t tid_slot, const Record& r){
	BufferFrame frame = bm->fixPage(tid.getPage(), true);
    SlottedPage* page = reinterpret_cast<SlottedPage*> frame.getData();
    page->remove(tid_slot);
	uint64_t offset = page->insert(r);
    Slot* slot = page->getFreeSlot();
    slot->length = r.getLen();
    slot->offset = offset;
}

SPSegment::~SPSegment(){
	bm->~BufferManager();
}
