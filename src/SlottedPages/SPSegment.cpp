#include <cerrno>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "SPSegment.hpp"

using namespace std;

SPSegment::SPSegment(){

}

TID SPSegment::insert(const Record& r){

    // Find page with enough space
    BufferFrame frame = bm.fixPage(???)

    SlottedPage* page = reinterpret_cast<SlottedPage*> frame.getData();
    // Reorder record ?

    // Write to page
    uint64_t offset = page->insert(r);
    Slot* slot = page->getFreeSlot();
    slot->length = Record->getLen();
    slot->offset = offset;

    // Return TID
}

bool SPSegment::remove(TID tid){
    BufferFrame frame = bm.fixPage(tid.getPage());
    SlottedPage* page = reinterpret_cast<SlottedPage*> frame.getData();
    page->remove(tid.getSlot());
    page->recompress();
}

Record SPSegment::lookup(TID tid) {
    BufferFrame frame = bm.fixPage(tid.getPage());
    SlottedPage* page = reinterpret_cast<SlottedPage*> frame.getData();
    Slot* slot = page->getSlot(tid.getSlot());

    if (slot->length == 0 && slot->offset == 0)
        return Record(0, nullptr); // Slot is empty, return empty record
    else
        return Record(slot->length, page->getRecordPtr(slot->offset));
}

bool SPSegment::update(TID tid, const Record& r){
    // If size doesn't change
        // Override data at slot address
    // If size is less
        // Override
        // [Optional] Re-compress
    // Else
        // If page has enough space
            // Remove
            // Compress
            // Write data
            // Update slot content
        // Else
            // replace with indirection to new position
}

SPSegment::~SPSegment(){

}
