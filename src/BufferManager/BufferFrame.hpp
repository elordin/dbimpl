#pragma once

#include <cstdint>
#include <pthread.h>

#define PAGESIZE 8192

enum FrameStatus {
    NEW   = 0x01,
    CLEAN = 0x02,
    DIRTY = 0x04
};


class BufferFrame {
 private:
    const uint64_t pageNo;
    pthread_rwlock_t* latch;
    int LSN;
    FrameStatus state;
    void* data;

    static uint64_t countPages;

 public:
    /**
     *  Constructor for an existing page
     */
    BufferFrame(uint64_t pageNo, void* data);

    /**
     *  Constructor for a new empty page
     *  Allocates memory for this page
     */
    BufferFrame(uint64_t pageNo);

    uint64_t getPageNo();

    int getLSN();

    void setLSN(int LSN);

    FrameStatus getState();

    void setState(FrameStatus state);

    void* getData();

    void writeBackChanges();
    ~BufferFrame();
};
