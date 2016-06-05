#pragma once

#include <cstdint>
#include <vector>

#include "Register.hpp"

class Print {
 private:
	
 public:
    
	//TODO: initialized with an input operator and an output stream
    Print();

	void open();

	// writes the next tuple, if there is any, to the output stream (in a human-readable format)
	bool next();

	std::vector<Register*> getOutput();

	void close();
    
    ~Print();
};
