#pragma once

#include <cstdint>

#include "BufferFrame.h"

#define PAGESIZE 8192

class BufferManager {
    private:

    public:
        BufferManager(uint pageCount);

        BufferFrame& fixPage(uint64_t pageId, bool exclusive);

        void unfixPage(BufferFrame& frame, bool isDirty)

        ~BufferManager();
};
