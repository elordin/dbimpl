#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "BufferManager.hpp"
#include "BufferFrame.hpp"
#include "HashTable.hpp"

// TODO
#define PAGE_PART_SIZE 32
#define SEGMENT_PART_SIZE 8 * sizeof(uint64_t) - PAGE_PART_SIZE


using namespace std;


BufferManager::BufferManager(uint pageCount)
  : table(new HashTable()),
    pageCount(pageCount),
    framesInMemory(pageCount) {
}


BufferFrame& BufferManager::fixPage(uint64_t pageId, bool exclusive) {
    if (!this->table->contains(pageId)) {
        this->load(pageId);
    }
    this->lock(pageId, exclusive);

    this->lru_list.remove(pageId);
    this->lru_list.push_back(pageId);

    return this->table->get(pageId);
}


void BufferManager::lock(uint64_t pageId, bool exclusive) {
    // TODO
}


bool hasXLocks(uint64_t pageId) {
    // TODO
    return true;
}

bool hasSLocks(uint64_t pageId) {
    // TODO
    return false;
}


int BufferManager::evict() {
    for (std::list<uint64_t>::iterator pageIdPtr = this->lru_list.begin();
            pageIdPtr != this->lru_list.end();
            pageIdPtr++) {
        uint64_t pageId = *pageIdPtr;
        if (!this->hasXLocks(pageId) && !this->hasSLocks(pageId)) {
            BufferFrame frame = this->table->get(pageId);
            this->table->removeItem(pageId);
            this->lru_list.remove(pageId);
            delete &frame;
            return 0;
        }
    }
    return -1;
}


void *BufferManager::load(uint64_t pageId) {
    if (this->table->size() < this->getPageCount()) {
        // TODO Prevent concurrent double loading of page
        if (this->table->contains(pageId)) {
            throw "Page is already in memory.";
        }

        std::string filename = this->getSegmentFilename(this->getSegmentId(pageId));

        int fd;

        if ((fd = open(filename.c_str(), O_CREAT | O_RDONLY)) < 0) {
            throw "Failed to open segment file.";
        }

        off_t offset = this->getPageOffset(pageId);
        // Redundantly ensure sufficient space.
        if (posix_fallocate(fd, offset, PAGESIZE) != 0) {
            throw "Failed to allocate sufficient file space.";
        }

        void *page = malloc(PAGESIZE);

        if (pread(fd, page, PAGESIZE, offset) < 0) {
            throw "Failed to load page.";
        }

        close(fd);

        return page;
    } else {
        if (this->evict() == 0) {
            return this->load(pageId);
        } else {
            throw "Failed to evict page. No free memory available.";
        }
    }
}


void BufferManager::write(uint64_t pageId) {
    BufferFrame frame = this->table->get(pageId);
    if (frame.getState() == DIRTY) {
        std::string filename = this->getSegmentFilename(this->getSegmentId(pageId));
        int fd;

        if ((fd = open(filename.c_str(), O_WRONLY)) < 0) {
            throw "Failed to open segment file.";
        }

        off_t offset = this->getPageOffset(pageId);

        if (posix_fallocate(fd, offset, PAGESIZE) != 0) {
            throw "Failed to allocate sufficient file space.";
        }

        if (pwrite(fd, frame.getData(), PAGESIZE, offset) < 0) {
            throw "Failed to write page to disc.";
        }
        close(fd);
    }
}


void BufferManager::unfixPage(BufferFrame& frame, bool isDirty) {
    // TODO
    // Release one lock.
            // Can only be a single write or multiple read locks. Write is removed, read is decreased.
    if (!this->hasXLocks(frame.getPageNo()) && !this->hasSLocks(frame.getPageNo())) {
        if (isDirty) {
            this->write(frame.getPageNo());
        }
    }
}


std::string BufferManager::getSegmentFilename(uint segmentId) {
    return std::to_string(segmentId);
}


uint64_t BufferManager::getSegmentId(uint64_t pageId) {
    // Extracts prefix of length SEGMENT_PART_SIZE from pageId by bitmask
    return (pageId >> PAGE_PART_SIZE) & (2 << SEGMENT_PART_SIZE - 1);
}


off_t BufferManager::getPageOffset(uint64_t pageId) {
    // Extracts suffix of length PAGE_PART_SIZE from pageId by bitmask
    return (pageId & (2 << PAGE_PART_SIZE - 1)) * PAGESIZE;
}


BufferManager::~BufferManager() {
    // TODO Delete hash table, deleting all buffer frames and in-memory pages
}
