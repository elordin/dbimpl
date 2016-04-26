#include <string.h>
#include <pthread.h>
#include <cstdlib>
#include "BufferFrame.hpp"

using namespace std;


BufferFrame::BufferFrame(uint64_t pageNo, void* data)
  : pageNo(pageNo) {
    // Ensures the data-block PAGESIZE large
    this->data = realloc(data, PAGESIZE);

    if (pthread_rwlock_init(this->latch, NULL)) {
        throw "Could not initialize latch.";
    }
}


BufferFrame::BufferFrame(uint64_t pageNo)
  : pageNo(pageNo) {
    this->data = malloc(PAGESIZE);
    if (pthread_rwlock_init(this->latch, NULL)) {
        throw "Could not initialize latch.";
    }
}


void BufferFrame::lock(bool exclusive) {
    if (exclusive) {
        if (pthread_rwlock_wrlock(this->latch) != 0) {
            throw "Failed to lock.";
        }
    } else {
        if (pthread_rwlock_rdlock(this->latch)) {
            throw "Failed to lock.";
        }
    }
}


int BufferFrame::tryLock(bool exclusive) {
    return exclusive  ? pthread_rwlock_trywrlock(this->latch) :
                        pthread_rwlock_tryrdlock(this->latch);
}


void BufferFrame::unlock() {
    if (pthread_rwlock_unlock(this->latch) != 0) {
        throw "Failed to unlock.";
    }
}


uint64_t BufferFrame::getPageNo() {
    return this->pageNo;
}


int BufferFrame::getLSN() {
    return this->LSN;
}


void BufferFrame::setLSN(int LSN) {
    if (LSN <= this->LSN) {
        throw "Can not lower the LSN.";
    }
    this->LSN = LSN;
}


FrameStatus BufferFrame::getState() {
    return this->state;
}


void BufferFrame::setState(FrameStatus state) {
    this->state = state;
}


void *BufferFrame::getData() {
    return this->data;
}


BufferFrame::~BufferFrame() {
    if (pthread_rwlock_trywrlock(this->latch) != 0) {
        throw "Trying to delete locked BufferFrame.";
    }
    free(this->data);
}
