#pragma once

#include <cstdint>
#include <vector>

#include "Register.hpp"

class Select {
 private:
	
 public:
    
	//TODO: initialized with an input operator, a register ID and a constant.
    Select();

	void open();

	bool next();

	std::vector<Register*> getOutput();

	void close();
    
    ~Select();
};
