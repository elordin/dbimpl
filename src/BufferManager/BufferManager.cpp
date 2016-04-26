#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <mutex>

#include "BufferManager.hpp"
#include "BufferFrame.hpp"
#include "HashTable.hpp"

// TODO
#define PAGE_PART_SIZE 48
#define SEGMENT_PART_SIZE 8 * sizeof(uint64_t) - PAGE_PART_SIZE


using namespace std;


BufferManager::BufferManager(uint pageCount)
  : table(new HashTable()),
    pageCount(pageCount) {
}


BufferFrame& BufferManager::fixPage(uint64_t pageId, bool exclusive) {
    this->table->lockBucket(pageId);
    if (!this->table->contains(pageId)) {
        // Insert an empty frame and lock it exclusively while performing disc I/O
        BufferFrame emptyFrame = BufferFrame(pageId);
        emptyFrame.setState(CLEAN);
        this->table->insert(pageId, emptyFrame);
        emptyFrame.lock(true); // X-Lock

        this->table->unlockBucket(pageId);

        this->load(pageId, emptyFrame.getData());
        emptyFrame.unlock();
    } else {
        this->table->unlockBucket(pageId);
    }

    BufferFrame& frame = this->table->get(pageId);
    frame.lock(exclusive);

    std::lock_guard<std::mutex> lock(this->lru_lock);
    this->lru_list.remove(pageId);
    this->lru_list.push_back(pageId);

    return frame;
}


void BufferManager::unfixPage(BufferFrame& frame, bool isDirty) {
    if (frame.tryLock(true) == 0) {
        this->write(frame);
    }
    frame.unlock();
}


int BufferManager::evict() {
    std::lock_guard<std::mutex> lock(this->lru_lock);
    /* TODO
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
    */
    return -1;
}


void BufferManager::load(uint64_t pageId, void *destination) {
    if (this->table->size() < this->getPageCount()) {

        std::string filename = this->getSegmentFilename(this->getSegmentId(pageId));
        int fd;

        if ((fd = open(filename.c_str(), O_CREAT | O_RDONLY)) < 0) {
            throw "Failed to open segment file.";
        }

        off_t offset = this->getPageOffset(pageId);

        // Ensure sufficient space.
        if (posix_fallocate(fd, offset, PAGESIZE) != 0) {
            throw "Failed to allocate sufficient file space.";
        }

        void *page = malloc(PAGESIZE);

        if (pread(fd, page, PAGESIZE, offset) < 0) {
            throw "Failed to load page.";
        }

        close(fd);
    } else {
        if (this->evict() == 0) {
            this->load(pageId, destination);
        } else {
            throw "Failed to evict page. No free memory available.";
        }
    }
}


void BufferManager::write(BufferFrame& frame) {
    // Assumes at least read lock on the frame

    // frame.lock(false); // S-Lock
    uint64_t pageId = frame.getPageNo();

    if (frame.getState() == DIRTY) {
        std::string filename = this->getSegmentFilename(this->getSegmentId(pageId));
        int fd;

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

    // frame.unlock();
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
    return (pageId & (2L << PAGE_PART_SIZE - 1)) * PAGESIZE;
}


BufferManager::~BufferManager() {
    // TODO Delete hash table, deleting all buffer frames and in-memory pages
}
