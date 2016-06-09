#include <cstdint>
#include <string>
#include <string.h>
#include <vector>

#include "Select.hpp"

using namespace std;

Select::Select(Operator* input, unsigned regId, string value)
: input(input),
  registerId(regId),
  value(new Register()) {
    this->value->setString(value);
}

Select::Select(Operator* input, unsigned regId, long value)
: input(input),
  registerId(regId),
  value(new Register()) {
    this->value->setInteger(value);
}

void Select::open() {
    this->input->open();
}

bool Select::next(){
    while(input->next()) {
        std::vector<Register*> tuples = input->getOutput();
        if (tuples.size() < this->registerId) {
            throw "Unknown attribute index in SELECT clause.";
        }

        Register* tuple = tuples[this->registerId];
        if (*this->value == *tuple) {
            return true;
        }
    }
    return false;
}

vector<Register*> Select::getOutput(){
    return this->input->getOutput();
}

void Select::close(){
    this->input->close();

}

Select::~Select(){

}
