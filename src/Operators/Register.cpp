#include <cstdint>
#include <cstring>
#include <string.h>

#include "Register.hpp"

using namespace std;

Register::Register()
: len(0) {

}

uint64_t Register::getInteger(){
    uint64_t v = *reinterpret_cast<uint64_t*>(this->data);
    return v;
}

void Register::setInteger(uint64_t value){
    if (this->len == 0) {
        this->data = (char*) malloc(sizeof(uint64_t) + sizeof(char));
    } else {
        this->data = (char*) realloc(sizeof(uint64_t) + sizeof(char));
    }
    memcpy(this->data, &value, sizeof(value));
    this->data[sizeof(uint64_t)] = '\0';
    len = sizeof(uint64_t);
}

string Register::getString(){
    return string(this->data);
}

void Register::setString(const string& value){
    if (this->len == 0) {
        this->data = (char*) malloc(value.length() * sizeof(char) + sizeof(char));
    } else {
        this->data = (char*) realloc(value.length() * sizeof(char) + sizeof(char));
    }
    memcpy(this->data, value.c_str(), value.length() * sizeof(char));
    this->data[value.length()] = '\0';
    len = value.length() * sizeof(char);
}

uint64_t Register::hash(){
    return this->hashfun(string(this->data));
}

bool Register::operator==(Register &that) {
    return strcmp(this->data, that.getString().c_str());
}

bool Register::operator==(Register *that) {
    return strcmp(this->data, that->getString().c_str());
}

Register::~Register(){
    free(this->data);
}
