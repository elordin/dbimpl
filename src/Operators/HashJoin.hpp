#pragma once

#include <cstdint>
#include <vector>

#include "Register.hpp"
#include "Operator.cpp"

class HashJoin : public Operator{
 private:
	
 public:
    
	//TODO: initialized with two input operators, and two register IDs. One ID is from the left side and one is from the right side. 
    HashJoin();

	void open();

	bool next();

	std::vector<Register*> getOutput();

	void close();
    
    ~HashJoin();
};
