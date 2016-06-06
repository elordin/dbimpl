#pragma once

#include <cstdint>
#include <vector>

#include "Register.hpp"
#include "Operator.cpp"

class Projection : public Operator{
 private:
	
 public:
    
	//TODO: initialized with an input operator and a list of register IDs (indexes into the register vector) it should project to
    Projection();

	void open();

	bool next();

	std::vector<Register*> getOutput();

	void close();
    
    ~Projection();
};
