#pragma once

#include <cstdint>
#include <vector>
#include "../SlottedPages/SPSegment.hpp"

#include "Register.hpp"

class TableScan {
 private:
	SPSegment relation;
 public:
    
	// which datatype is a relation??
    TableScan(SPSegment relation);

	void open();

	// reads the next tuple, if there is any
	bool next();

	// returns the value of the current tuple
	std::vector<Register*> getOutput();

	void close();
    
    ~TableScan();
};
