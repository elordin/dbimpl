#include <string.h>
#include <pthread.h>

#include <cerrno>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "BufferFrame.hpp"

using namespace std;


BufferFrame::BufferFrame(uint64_t pageNo, void* data)
  : pageNo(pageNo),
    LSN(0),
    state(CLEAN) {
    // Ensures the data-block PAGESIZE large
    this->data = realloc(data, PAGESIZE);

    if (pthread_rwlock_init(&this->latch, NULL) != 0) {
        perror("Could not initialize latch.");
        perror(nullptr);
        throw "Could not initialize latch.";
    }
}


BufferFrame::BufferFrame(uint64_t pageNo)
  : pageNo(pageNo),
    LSN(0),
    state(NEW) {
    this->data = malloc(PAGESIZE);
    if (pthread_rwlock_init(&this->latch, NULL) != 0) {
        perror("Could not initialize latch.");
        throw "Could not initialize latch.";
    }
}


void BufferFrame::lock(bool exclusive) {
    if (exclusive) {
        if (pthread_rwlock_wrlock(&this->latch) != 0) {
            perror("ERROR");
            perror("Failed to lock exclusive.");
            throw "Failed to lock.";
        }
    } else {
        if (pthread_rwlock_rdlock(&this->latch) != 0) {
            perror("ERROR");
            perror("Failed to lock non exclusive.");
            throw "Failed to lock.";
        }
    }
}


int BufferFrame::tryLock(bool exclusive) {
    return exclusive  ? pthread_rwlock_trywrlock(&this->latch) :
                        pthread_rwlock_tryrdlock(&this->latch);
}


void BufferFrame::unlock() {
    if (pthread_rwlock_unlock(&this->latch) != 0) {
        perror("Failed to unlock.");
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
        perror("Can not lower the LSN.");
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
    if (pthread_rwlock_trywrlock(&this->latch) != 0) {
        perror("Tying to delete locked BufferFrame.");
        throw "Trying to delete locked BufferFrame.";
    }
    free(this->data);
}
