#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <string.h>

#include "Register.hpp"
#include "Operator.cpp"

class Select : public Operator {
 private:
	Operator* input;
	Register* attribute;
	std::string value;
 public:

	// initialized with an input operator, a register ID and a constant.
    Select(Operator* input, Register* attribute, std::string value);

	void open();

	bool next();

	std::vector<Register*> getOutput();

	void close();

    ~Select();
};
