#pragma once

#include <cstdint>
#include <list>
#include <mutex>

#include "BufferFrame.hpp"
#include "HashTable.hpp"

class BufferManager {
 private:
    HashTable* table;

    // List ordered by recency of usage, LRU at the end.
    std::list<uint64_t> lru_list;
    std::mutex lru_lock;

    // Max number of pages.
    uint pageCount;

 public:
    /**
     *  Constructor
     *  pageCount is the max. number of pages held in memory
     */
    BufferManager(uint pageCount);

    /**
     *  Loads page form disc into main memory.
     *  Manages locking.
     *  Evicts pages when no space is available.
     *  Guarantees page to be in memory when the call returns.
     */
    BufferFrame& fixPage(uint64_t pageId, bool exclusive);

    /**
     *  States that the caller no longer needs the page in memory
     *  Page can thus be evicted if necessary if no on else accesses it
     */
    void unfixPage(BufferFrame& frame, bool isDirty);

    /**
     *  Evicts the next page according to eviction strategy.
     *  Returns 0 on success, -1 on error.
     */
    // Preferred strategy is TinyLFU: http://arxiv.org/pdf/1512.00727.pdf
    // https://github.com/ben-manes/caffeine/wiki/Efficiency
    int evict();

    ~BufferManager();

    /**
     *  Loads page from disc to position in memory.
     */
    void load(uint64_t pageId, void *destination);

    /**
     *  Writes page to disc.
     */
    void write(BufferFrame& frame);

    /**
     *  Resolves file name for given segment.
     */
    std::string getSegmentFilename(uint segmentId);

    /**
     *  Returns what segment a given page is in.
     */
    uint64_t getSegmentId(uint64_t pageId);

    /**
     *  Returns offset of given page within its segment.
     */
    off_t getPageOffset (uint64_t pageId);

    uint getPageCount();
};
