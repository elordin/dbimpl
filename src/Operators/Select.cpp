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
        std::vector<Register*> tuple = input->getOutput();
        if (tuple.size() < this->registerId) {
            throw "Unknown attribute index in SELECT clause.";
        }

        Register* comparisonValue = tuple[this->registerId];
        if (*this->value == *comparisonValue) {
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
