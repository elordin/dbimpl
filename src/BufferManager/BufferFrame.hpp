#pragma once

#include <stdint.h>
#include "HashTable.hpp"

class BufferFrame {
    private:
	uint64_t pageNo;
	bool locked;
	void* data;

	static uint64_t countPages;
    public:
        BufferFrame(void* dataForFrame);

        void* getData();
	void writeBackChanges();
	uint64_t getPageNo(){return pageNo;};

        ~BufferFrame();
};
