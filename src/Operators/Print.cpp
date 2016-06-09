#include <cstdint>
#include <iostream>

#include "Print.hpp"

using namespace std;

Print::Print(Operator* input) : input(input) {}

void Print::open(){
    this->input->open();
}

bool Print::next(){
    return this->input->next();
}

vector<Register*> Print::getOutput(){
    vector<Register*> tuple = this->input->getOutput();
    std::cout << "[";
    for (int i = 0; i < tuple.size(); i++) {
        std::cout << tuple[i]->toString() << ", ";
    }
    std::cout << "]" << std::endl;

    return tuple;
}

void Print::close(){
    this->input->close();
}

Print::~Print(){

}
