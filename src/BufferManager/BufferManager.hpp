#pragma once

#include <cstdint>

#include "BufferFrame.hpp"
#include "HashTable.hpp"

#define PAGESIZE 8192

class BufferManager {
    private:
	HashTable table;
	int framesInMemory;
    public:
        BufferManager(uint pageCount);

        BufferFrame& fixPage(uint64_t pageId, bool exclusive);

        void unfixPage(BufferFrame& frame, bool isDirty);

        ~BufferManager();
};
