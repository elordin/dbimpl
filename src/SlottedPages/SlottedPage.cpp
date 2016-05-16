#include <cassert>
#include "SlottedPage.hpp"


#define PAGE_SIZE 8192 // TODO Duplicate to BufferManager
#define SLOT_COUNT 128 // TODO sure about that value?

using namespace std;

struct Header {

};


SlottedPage::SlottedPage(uint64_t pageId)
 : pageId(pageId) {
    header          = (char*) this;
    firstSlot       = (Slot*) this + sizeof(Header);
    firstEmptySlot  = firstSlot;
    slotEnd         = reinterpret_cast<Slot*>(reinterpret_cast<uint64_t>(firstSlot) + SLOT_COUNT * sizeof(Slot));
    end             = (char*) this + PAGE_SIZE;
    freeSpace       = end;
}

uint64_t SlottedPage::insert(const Record& r){
    if (this->getFreeSpaceOnPage() < r.getLen()) throw "Insufficient space.";

    Slot* slot = firstEmptySlot;

    assert(firstEmptySlot < slotEnd);

    char* destination = this->freeSpace - r.getLen();
    memcpy(destination, &r, r.getLen());

    uint64_t slotNum = ((char*) slot - header) / sizeof(Slot);

    slot->length = r.getLen();
    slot->offset = (uint64_t) destination - (uint64_t) slot; // Slot::offset is the offset between slot and data
    slot->moved  = false;
    slot->tid    = pageId | slotNum; // TODO

    for (Slot* s = firstEmptySlot; s < slotEnd; s += sizeof(Slot)) {
        firstEmptySlot = s;
        if (s->isEmpty()) break;
    }

    freeSpace -= r.getLen();

    return slot->tid.getTID();
}

void SlottedPage::remove(uint64_t slotNum) {
    Slot* slot = this->getSlot(slotNum);
    slot->length = 0;
    slot->offset = 0;
    slot->moved  = false;

    if (firstEmptySlot > slot) {
        firstEmptySlot = slot;
    }
}



uint64_t SlottedPage::recompress() {
    // TODO
    return 0;
}


Slot* SlottedPage::getSlot(uint64_t slotNum){
    return firstSlot + slotNum * sizeof(Slot);
}


Slot* SlottedPage::getFreeSlot(){
    return firstEmptySlot;
}

Record* SlottedPage::getRecordPtr(uint64_t slotNum){
    Slot* slot = getSlot(slotNum);
    if (slot->isEmpty()) throw "Empty slot doesn't have an associated record.";
    return reinterpret_cast<Record*>(slot + slot->offset);
}

unsigned SlottedPage::getFreeSpaceOnPage(){
    return ((uint64_t) this->freeSpace - (uint64_t) this->slotEnd);
}

SlottedPage::~SlottedPage(){
}
