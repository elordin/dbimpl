#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <mutex>
#include <cerrno>
#include <cstdio>

#include "BufferManager.hpp"
#include "BufferFrame.hpp"
#include "HashTable.hpp"


#define PAGE_PART_SIZE 8
#define SEGMENT_PART_SIZE (8 * sizeof(uint64_t) - PAGE_PART_SIZE)


using namespace std;


BufferManager::BufferManager(uint pageCount)
  : table(new HashTable()),
    pageCount(pageCount) {
}


BufferFrame& BufferManager::fixPage(uint64_t pageId, bool exclusive) {
    std::cout << "Fixing page: " << pageId << std::endl;
    this->table->lockBucket(pageId);
    if (!this->table->contains(pageId)) {

        // Insert an empty frame and lock it exclusively while performing disc I/O
        BufferFrame* emptyFrame = new BufferFrame(pageId);

        emptyFrame->setState(CLEAN);
        this->table->insert(pageId, emptyFrame);
        emptyFrame->lock(true); // X-Lock

        this->table->unlockBucket(pageId);

        // Load from disc (slow I/O), only the single frame is locked.
        this->load(pageId, emptyFrame->getData());
        emptyFrame->unlock();
    } else {
        this->table->unlockBucket(pageId);
    }

    // ??? Could the page get evicted before this get is called?
    BufferFrame& frame = this->table->get(pageId);
    frame.lock(exclusive);

    std::lock_guard<std::mutex> lock(this->lru_lock);
    this->lru_list.remove(pageId);
    this->lru_list.push_back(pageId);

    return frame;
}


void BufferManager::unfixPage(BufferFrame& frame, bool isDirty) {
    std::cout << "Unfixing page: " << frame.getPageNo() << std::endl;
    // Return the one lock this thread held.
    frame.unlock();
    // If then no one with locks is left and the page is dirty, write it to disc.
    if (isDirty && frame.tryLock(true) == 0) {
        this->write(frame);
        frame.unlock();
    }
}


int BufferManager::evict() {
    std::lock_guard<std::mutex> lock(this->lru_lock);
    // for ( each frame ) {
        // if (frame->tryLock(true) == 0) {
            // Remove from table
            // Remove from lru list
            // delete frame;
            // return 0
        // }
    // }
    return -1;
    // Implicit unlock of lru_lock at end of scope.
}


void BufferManager::load(uint64_t pageId, void *destination) {
    if (this->table->size() < this->pageCount) {

        std::string filename = this->getSegmentFilename(this->getSegmentId(pageId));
        int fd;

        if ((fd = open(filename.c_str(), O_CREAT | O_RDWR)) < 0) {
            std::cout << "Failed to open segment file" << std::endl;
            perror("\tERROR");
            throw "Failed to open segment file.";
        }

        off_t offset = this->getPageOffset(pageId);

        std::cout << "FD: " << fd << " Offset: " << offset << " Pagesize: " << PAGESIZE << std::endl;

        // Ensure sufficient space.
        int err;
        if ((err = posix_fallocate(fd, offset, PAGESIZE)) != 0) {
            std::cout << "Failed to allocate sufficient file space." << std::endl;
            std::cout << "\t> " << strerror(err) << std::endl;
            perror("\tERROR");
            throw "Failed to allocate sufficient file space.";
        }

        void *page = malloc(PAGESIZE);

        if (pread(fd, page, PAGESIZE, offset) < 0) {
            std::cout << "Failed to load page." << std::endl;
            throw "Failed to load page.";
        }

        close(fd);
    } else {
        if (this->evict() == 0) {
            this->load(pageId, destination);
        } else {
            std::cout << "Failed to evict page. No free memory available." << std::endl;
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

        if ((fd = open(filename.c_str(), O_WRONLY)) < 0)
         {
            std::cout << "Failed to open segment file." << std::endl;
            throw "Failed to open segment file.";
        }

        off_t offset = this->getPageOffset(pageId);

        if (posix_fallocate(fd, offset, PAGESIZE) != 0) {
            std::cout << "Failed to allocate sufficient file space." << std::endl;
            throw "Failed to allocate sufficient file space.";
        }

        if (pwrite(fd, frame.getData(), PAGESIZE, offset) < 0) {
            std::cout << "Failed to write page to disc." << std::endl;
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
    return (pageId >> PAGE_PART_SIZE) & ((2L << SEGMENT_PART_SIZE) - 1);
}


off_t BufferManager::getPageOffset(uint64_t pageId) {

    // Extracts suffix of length PAGE_PART_SIZE from pageId by bitmask
    return (pageId & ((2L << SEGMENT_PART_SIZE) - 1)) * PAGESIZE;
}


BufferManager::~BufferManager() {
    delete this->table;
}
