#pragma once

#include <cstdint>
#include <vector>
#include "../SlottedPages/SPSegment.hpp"

#include "Register.hpp"
#include "Operator.cpp"

class TableScan : public Operator{
 private:
	SPSegment* input;
	uint64_t tid;
	uint64_t limit;
	const char* currentTuple;
 public:

    TableScan(SPSegment* relation);

	void open();

	// reads the next tuple, if there is any
	bool next();

	// returns the value of the current tuple
	std::vector<Register*> getOutput();

	void close();

    ~TableScan();
};
