#pragma once

#include <cstdint>
#include <list>

#include "BufferFrame.hpp"
#include "HashTable.hpp"

class BufferManager {
 private:
    HashTable* table;

    // List ordered by recency of usage, LRU at the end.
    std::list<uint64_t> lru_list;
    
    // Max number of pages.
    uint pageCount;

    // TODO What is this for?
    int framesInMemory;

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
     *  Evicts the next page according to eviction strategy.
     *  Returns 0 on success, -1 on error.
     */
    // Preferred strategy is TinyLFU: http://arxiv.org/pdf/1512.00727.pdf
    // https://github.com/ben-manes/caffeine/wiki/Efficiency
    int evict();

    /**
     *  States that the caller no longer needs the page in memory
     *  Page can thus be evicted if necessary if no on else accesses it
     */
    void unfixPage(BufferFrame& frame, bool isDirty);

    ~BufferManager();

    /**
     *  Returns whether the given page is currently locked exclusively.
     */
    bool hasXLocks(uint64_t pageId);
    
    /** 
     *  Returns whether the given page is currently locked shared.
     */
    bool hasSLocks(uint64_t pageId);

    /**
     *  Sets S or X lock on a given page.
     *  Blocks until lock can be set.
     */
    void lock(uint64_t pageId, bool exclusive);

    /**
     *  Loads page from disc into memory.
     */
    void *load(uint64_t pageId);

    /**
     *  Writes page to disc.
     */
    void write(uint64_t pageId);

    /**
     *  Resolves file name for given segment.
     */
    std::string getSegmentFilename(uint segmentId);

    /**
     *  Returns what segment a given page is in.
     */
    uint getSegmentId(uint64_t pageId);

    /**
     *  Returns offset of given page within its segment.
     */
    off_t getPageOffset (uint64_t pageId);

    uint getPageCount();
};
