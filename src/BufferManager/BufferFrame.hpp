#pragma once

#include <cstdint>
#include <pthread.h>
#include "../constants.hpp"

enum FrameStatus {
    NEW   = 0x01,
    CLEAN = 0x02,
    DIRTY = 0x04
};


class BufferFrame {
 private:
    const uint64_t pageNo;
    pthread_rwlock_t latch;
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

    /**
     *  Locks the BufferFrame using a pthread_rwlock
     *  Blocks until lock could be aquired.
     *  Uses write lock when exclusive is true, read lock otherwise.
     */
    void lock(bool exclusive);

    /**
     *  Tries to get a lock on this BufferFrame.
     *  Does not block, returns -1 if no lock could be aquired, 0 otherwise.
     */
    int tryLock(bool exclusive);

    /**
     *  Releases one lock (read or write).
     */
    void unlock();

    uint64_t getPageNo();

    int getLSN();

    void setLSN(int LSN);

    FrameStatus getState();

    void setState(FrameStatus state);

    void* getData();

    ~BufferFrame();
};
