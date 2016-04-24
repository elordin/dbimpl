#include <string.h>
#include <pthread.h>
#include <cstdlib>
#include "BufferFrame.hpp"

using namespace std;


BufferFrame::BufferFrame(uint64_t pageNo, void* data)
  : pageNo(pageNo) {
    // TODO If someone else ensures the datablock at *data is PAGESIZE large, we can just reference
    this->data = malloc(PAGESIZE);
    memcpy(this->data, data, PAGESIZE);

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
    // Locks?
    free(this->data);
}
