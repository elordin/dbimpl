#pragma once

#include <cstdint>
#include <vector>
#include "../SlottedPages/SPSegment.hpp"

#include "Register.hpp"

class TableScan {
 private:
	SPSegment input;
	uint64_t tid;
	uint64_t limit;
	const char* currentTuple;
 public:
    
	// right datatype??
    TableScan(SPSegment relation);

	void open(SPSegment relation);

	// reads the next tuple, if there is any
	bool next();

	// returns the value of the current tuple
	std::vector<Register*> getOutput();

	void close();
    
    ~TableScan();
};
