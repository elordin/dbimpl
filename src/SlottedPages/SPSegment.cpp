#include <cerrno>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "SPSegment.hpp"

using namespace std;

#define HEADER_SIZE sizeof(unsigned) + sizeof(unsigned) + sizeof(Slot*) + sizeof(void*) + sizeof(uint64_t)

SPSegment::SPSegment(){

}

Slot SPSegment::getSlot(uint64_t slotNumber) {
    return reinterpret_cast<Slot>(this + HEADER_SIZE + slotNumber * sizeof(Slot));
}

TID SPSegment::insert(const Record& r){
    // Get a page with enough space from BufferManager
    // Reorder record
    // Write to page
    // Update slot
    // Return TID
}

bool SPSegment::remove(TID tid){
    // Remove data
    // Empty slot
    // Re-compress data
}

Record SPSegment::lookup(TID tid) {
    Slot slot = this->getSlot(tid.getSlot());
    if (slot->length == 0 && slot->offset == 0)
        return Record(0, nullptr); // Slot is empty, return empty record
    else
        return Record(slot->length, this + SEGMENT_SIZE - slot->offset);
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
