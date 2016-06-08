#pragma once

#include <cstdint>
#include <vector>

#include "Register.hpp"
#include "Operator.cpp"

class Projection : public Operator{
 private:
    Operator* input;
    vector<unsigned> ids;
 public:

    Projection(Operator* input, std::vector<unsigned> ids);

	void open();

	bool next();

	std::vector<Register*> getOutput();

	void close();

    ~Projection();
};
