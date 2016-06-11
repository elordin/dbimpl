#include <cassert>
#include <exception>
#include "SlottedPage.hpp"


#define PAGE_SIZE 8192 // TODO Duplicate to BufferManager

class InsufficientSpaceException: public std::exception {
    virtual const char* what() const throw() { return "Insufficient space."; }
} InsufficientSpaceException;


SlottedPage::Header::Header() :
    LSN(0),
    slotCount(0),
    firstFreeSlot(reinterpret_cast<Slot*>(this + sizeof(Header))),
    dataStart(reinterpret_cast<char*>(this + PAGE_SIZE)),
    freeSpace(PAGE_SIZE - sizeof(Header) - sizeof(Slot)) {}


SlottedPage::SlottedPage() : header(new Header()) {}


unsigned SlottedPage::insert(const Record& r) {
    // TODO Check space requirement
    // if (this->getFreeSpaceOnPage() < r.getLen()) throw InsufficientSpaceException;

    Slot* slot = this->header->firstFreeSlot;

    // Dump data
    char* destination = this->header->dataStart - r.getLen();
    memcpy(destination, &r, r.getLen());

    // Update slot
    unsigned slotNum = ((uint64_t) slot - (uint64_t) this - sizeof(header)) / sizeof(Slot);
    *slot = Slot((uint64_t) destination - (uint64_t) slot, r.getLen(), false);

    // TODO Update firstFreeSlot
    /*
    for (Slot* s = firstEmptySlot; s < slotEnd; s += sizeof(Slot)) {
        firstEmptySlot = s;
        if (s->isEmpty()) break;
    }
    */

    // Update header
    this->header->freeSpace -= r.getLen();
    this->header->slotCount++;

    return slotNum;
}


void SlottedPage::remove(TID tid) {
    Slot* slot = this->getSlot(tid);
    this->header->freeSpace += slot->length();
    this->header->slotCount--;

    *slot = Slot();

    if (this->header->firstFreeSlot > slot) {
        this->header->firstFreeSlot = slot;
    }
}


uint64_t SlottedPage::recompress() {
    // TODO
    return 0;
}


SlottedPage::Slot* SlottedPage::getSlot(TID tid) {
    return reinterpret_cast<SlottedPage::Slot*>(this + sizeof(Header) + tid.getSlot() * sizeof(Slot));
}

/*

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
*/

SlottedPage::~SlottedPage() {
    delete this->header;
}
