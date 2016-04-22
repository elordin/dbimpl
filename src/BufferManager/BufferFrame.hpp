#pragma once

#include <cstdint>


enum FrameStatus {
    NEW   = 0x01,
    CLEAN = 0x02,
    DIRTY = 0x04
};


class BufferFrame {
 private:
    const uint64_t pageNo;
    bool latch;
    int LSN;
    FrameStatus state;
    void* data;

    static uint64_t countPages;

 public:
    BufferFrame(void* data);

    /**
     *  Constructor
     *  Allocates PAGESIZE memory for this page
     */
    BufferFrame();

    /** Getter method */
    void* getData();

    /** Getter method */
    uint64_t getPageNo();

    void writeBackChanges();
    ~BufferFrame();
};
