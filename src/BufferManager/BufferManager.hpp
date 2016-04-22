#pragma once

#include <cstdint>

#include "BufferFrame.hpp"
#include "HashTable.hpp"

class BufferManager {
 private:
    HashTable* table;
    int framesInMemory;

 public:
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

    void unfixPage(BufferFrame& frame, bool isDirty);

    ~BufferManager();
};
