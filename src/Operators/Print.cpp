#include <cstdint>
#include <iostream>

#include "Print.hpp"

using namespace std;

Print::Print(Operator* input, std::ostream* target) : input(input), target(target) {}

void Print::open(){
    this->input->open();
}

bool Print::next(){
    return this->input->next();
}

vector<Register*> Print::getOutput(){
    vector<Register*> tuple = this->input->getOutput();
    *this->target << "[";
    for (int i = 0; i < tuple.size(); i++) {
        *this->target << tuple[i]->toString() << ", ";
    }
    *this->target << "]" << std::endl;

    return tuple;
}

void Print::close(){
    this->input->close();
}

Print::~Print(){

}
