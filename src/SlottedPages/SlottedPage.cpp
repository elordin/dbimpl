#include "SlottedPage.hpp"

#define PAGE_SIZE 8192; // TODO Duplicate to BufferManager
#define SLOT_COUNT 256; // TODO sure about that value?

using namespace std;

struct Header {

};


SlottedPage::SlottedPage(uint64_t pageId)
 : pageId(pageId) {

    header          = this;
    firstSlot       = this + sizeof(Header);
    firstEmptySlot  = firstSlot;
    slotEnd         = firstSlot + SLOT_COUNT * sizeof(Slot);
    end             = this + PAGE_SIZE;
    freeSpace       = end;

}

uint64_t SlottedPage::insert(const Record& r){
    if (this->getFreeSpaceOnPage() < r.getLen()) throw "Insufficient space.";

    Slot* slot = firstEmptySlot;

    char* destination = this->freeSpace - r.getLen();
    memcpy(r, destination, r.getLen());

    uint64_t slotNum = (slot - header) / sizeof(Slot);

    slot.length = r.getLen();
    slot.offset = destination - slot; // Slot::offset is the offset between slot and data
    slot.moved  = false;
    slot.tid    = pageId | slotNum; // TODO

    firstEmptySlot += sizeof(Slot); // TODO What when freeSlot is one between two full slots due to remove?

    freeSpace -= r.getLen();

    return slot.tid;
}

void SlottedPage::remove(uint64_t slotNum) {
    Slot* slot = this->getSlot(slotNum);
    slot.length = 0;
    slot.offset = 0;
    slot.moved  = false;

    if (firstEmptySlot > slot) {
        firstEmptySlot = slot;
    }
}

uint64_t SlottedPage::recompress(){
}


Slot* SlottedPage::getSlot(uint64_t slotNum){
    return firstSlot + slotNum * sizeof(Slot);
}


Slot* SlottedPage::getFreeSlot(){
    return firstEmptySlot;
}

Record* SlottedPage::getRecordPtr(uint64_t slotNum){
    Slot* slot = firstSlot + slotNum * sizeof(Slot);
    return reinterpret_cast<Record*>(slot + slot->offset);
}

unsigned SlottedPage::getFreeSpaceOnPage(){
    return this->freeSpace - this->slotEnd;
}

SlottedPage::~SlottedPage(){
}
