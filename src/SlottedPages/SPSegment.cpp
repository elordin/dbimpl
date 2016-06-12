#include <cerrno>
#include <cstdio>
#include <iostream>
#include <cstdlib>


#include "../parser/Types.hpp"
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
        if (lastPage > 1l << 48) throw "Max page number reached.";
    }

    // TODO Reorder record ?

    // Write to page
    unsigned slotNum = page->insert(r);

    // TODO update FSI

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
        // TODO Update freeSpace
        // TODO update FSI
	} else {
		unsigned freeSpaceOnPage = page->freeSpace();
    	if (freeSpaceOnPage >= len_new) {
			page->remove(tid.getSlot());
			page->insert(r);
    	} else {
			// TODO: indirection to new position
            uint64_t sndPageId = this->lastPage + 1;
            for (auto it = this->fsi.rbegin(); it != this->fsi.rend(); it++) {
                if (it->second >= r.getLen()) {
                    sndPageId = it->first;
                    break;
                }
            }
            BufferFrame sndFrame = bm->fixPage(sndPageId, true);
            SlottedPage* sndPage = reinterpret_cast<SlottedPage*>(sndFrame.getData());
            if (sndPageId > this->lastPage) {
                *sndPage = SlottedPage();
                this->lastPage++;
                if (lastPage > 1l << 48) throw "Max page number reached.";
            }

            Slot* fstSlot = page->getSlot(tid.getSlot());
            unsigned fstSpace = page->remove(tid);
            // TODO update FSI
            assert(fstSlot->isEmpty());

            unsigned sndSlotNum = sndPage->insert(r);
            // TODO update FSI

            *fstSlot = Slot(TID(sndPageId, sndSlotNum));

            bm->unfixPage(sndFrame, true);
		}
	}
	bm->unfixPage(frame, true);
	return true;
}


std::vector<Register*> SPSegment::toRegisterVector(const char* data) {
    Schema::Relation relation = this->relation();

    unsigned nextVarLength = 0;
    unsigned offset = 0;
    vector<Register*> result = vector<Register*>();

    for (auto attr = relation.attributes.begin(); attr != relation.attributes.end(); attr++) {
        switch ((*attr).type) {
            case Types::Tag::Integer:
                nextVarLength += sizeof(int);
                break;
            case Types::Tag::Char:
                nextVarLength += sizeof(unsigned);
                break;
        }
    }

    for (auto attr = relation.attributes.begin(); attr != relation.attributes.end(); attr++) {
        Register* reg = new Register();

        switch ((*attr).type) {
            case Types::Tag::Integer:
                reg->setInteger(*(reinterpret_cast<const int*>(data + offset)));

                offset += sizeof(int);
                break;
            case Types::Tag::Char:
                const char* str = data + nextVarLength;
                const unsigned endOffset = *(reinterpret_cast<const unsigned*>(data + offset));
                reg->setString(string(str, offset + endOffset - nextVarLength));

                offset += sizeof(unsigned);
                break;
        }
        result.push_back(reg);
    }

    return result;
}

Schema::Relation SPSegment::relation() {
    // Get Metadata Segment
    // Load Relation from Metadata Segment
}


SPSegment::~SPSegment() {}
