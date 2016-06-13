#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>

#include "Register.hpp"
#include "Operator.cpp"

class HashJoin : public Operator{
 private:
	Operator* leftOp;
	Operator* rightOp;
	unsigned regIdLeft;
	unsigned regIdRight;
	vector<Register*> result;
	std::unordered_map<uint64_t, vector<Register*>> hashTable;
 public:
    
	//initialized with two input operators, and two register IDs. One ID is from the left side and one is from the right side. 
    HashJoin(Operator* leftOp, Operator* rightOp, unsigned regIdLeft, unsigned regIdRight);

	void open();

	bool next();

	std::vector<Register*> getOutput();

	void close();
    
    ~HashJoin();
};
