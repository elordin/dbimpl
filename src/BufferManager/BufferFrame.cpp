#include <cstdlib>

#include "BufferFrame.hpp"

using namespace std;

#define PAGESIZE 8192


//TODO: control if the page number rises
BufferFrame::BufferFrame(void* data)
  : pageNo(++BufferFrame::countPages),
    latch(false),
    data(data) {
    // Ensure data is PAGESIZE large
}


BufferFrame::BufferFrame()
  : pageNo(++BufferFrame::countPages),
    latch(false) {
    this->data = malloc(PAGESIZE);
}


uint64_t BufferFrame::getPageNo() {
    return this->pageNo;
}


void *BufferFrame::getData() {
    return this->data;
}


void writeBackChanges() {
    // insertItem(this);
}


//TODO: Destructor
BufferFrame::~BufferFrame() {
}
