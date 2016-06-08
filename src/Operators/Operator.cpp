#include<vector>

#include "Register.hpp"

using namespace std;

// Base class Operator
class Operator
{
public:

    virtual void open() = 0;

    virtual void close() = 0;

	virtual bool next() = 0;

	virtual vector<Register*> getOutput() = 0;

	virtual ~Operator(){}

};
