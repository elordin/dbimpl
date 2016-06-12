#include <cassert>
#include <exception>

#include "SlottedPage.hpp"
#include "../constants.hpp"


class InsufficientSpaceException: public std::exception {
    virtual const char* what() const throw() { return "Insufficient space."; }
} InsufficientSpaceException;


SlottedPage::Header::Header() :
    LSN(0),
    slotCount(0),
    firstFreeSlot(reinterpret_cast<Slot*>(this + sizeof(Header))),
    dataStart(reinterpret_cast<char*>(this + PAGESIZE)),
    freeSpace(PAGESIZE - sizeof(Header) - sizeof(Slot)) {}


SlottedPage::SlottedPage() : header(new Header()) {}


unsigned SlottedPage::insert(const Record& r) {
    if (this->header->freeSpace < r.getLen()) throw InsufficientSpaceException;

    // TODO if (actual space is insufficient) {
        if (this->recompress() < r.getLen()) throw InsufficientSpaceException;
    // }

    Slot* slot = this->header->firstFreeSlot;

    // Dump data
    char* destination = this->header->dataStart - r.getLen();
    memcpy(destination, &r, r.getLen());

    // Update slot
    unsigned slotNum = ((uint64_t) slot - (uint64_t) this - sizeof(header)) / sizeof(Slot);
    *slot = Slot((uint64_t) destination - (uint64_t) slot, r.getLen(), false);

    // Update firstFreeSlot
    int i;
    for (this->header->firstFreeSlot = reinterpret_cast<Slot*>(this + sizeof(header)), i = 0;
            i <= this->header->slotCount;
            i++, this->header->firstFreeSlot += sizeof(Slot)) {
        if (this->header->firstFreeSlot->isEmpty()) {
            break;
        }
    }

    // Update header
    this->header->freeSpace -= r.getLen();
    this->header->slotCount++;

    return slotNum;
}


unsigned SlottedPage::remove(TID tid) {
    Slot* slot = this->getSlot(tid);
    this->header->freeSpace += slot->length();
    this->header->slotCount--;

    *slot = Slot();

    if (this->header->firstFreeSlot > slot) {
        this->header->firstFreeSlot = slot;
    }

    return this->header->freeSpace;
}


unsigned SlottedPage::recompress() {
    // TODO
    return this->header->freeSpace;
}


Slot* SlottedPage::getSlot(TID tid) {
    return reinterpret_cast<Slot*>(this + sizeof(Header) + tid.getSlot() * sizeof(Slot));
}


/*
Record* SlottedPage::getRecordPtr(uint64_t slotNum){
    Slot* slot = getSlot(slotNum);
    if (slot->isEmpty()) throw "Empty slot doesn't have an associated record.";
    return reinterpret_cast<Record*>(slot + slot->offset);
}
*/

unsigned SlottedPage::freeSpace() {
    return this->header->freeSpace;
}

SlottedPage::~SlottedPage() {
    delete this->header;
}
