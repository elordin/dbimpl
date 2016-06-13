#pragma once

#include <cstdint>
#include <vector>
#include <ostream>

#include "Register.hpp"
#include "Operator.cpp"

class Print : public Operator{
 private:
    Operator* input;
    std::ostream* target;
 public:

    Print(Operator* input, std::ostream* ostream);

	void open();

	// writes the next tuple, if there is any, to the output stream (in a human-readable format)
	bool next();

	std::vector<Register*> getOutput();

	void close();

    ~Print();
};
