#include <cstdint>
#include <vector>

#include "Projection.hpp"

using namespace std;

Projection::Projection(Operator* input, std::vector<unsigned> ids) {
    this->input = input;
    this->ids = ids;
}

void Projection::open() {
    this->input->open();
}

bool Projection::next(){
    return input->next();
}

vector<Register*> Projection::getOutput(){
    vector<Register*> result = vector<Register*>(ids.size());
    vector<Register*> tuple = input->getOutput();
    for (int i = 0; i < ids.size(); i++) {
        result[i] = tuple[ids[i]];
    }
    return result;
}

void Projection::close(){
    this->input->close();
}

Projection::~Projection(){

}
