#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string.h>

#include "Register.hpp"

using namespace std;


Register::Register()
: len(0), type(TANY) {

}

long Register::getInteger(){
    return *reinterpret_cast<long*>(this->data);
}

void Register::setInteger(long value){
    if (this->len == 0) {
        this->data = (char*) malloc(sizeof(long) + sizeof(char));
    } else {
        this->data = (char*) realloc(this->data, sizeof(long) + sizeof(char));
    }
    memcpy(this->data, &value, sizeof(value));
    this->data[sizeof(long)] = '\0';
    len = sizeof(long);
    this->type = TINT;
}

string Register::getString(){
    return string(this->data);
}

void Register::setString(const string& value){
    if (this->len == 0) {
        this->data = (char*) malloc(value.length() * sizeof(char) + sizeof(char));
    } else {
        this->data = (char*) realloc(this->data, value.length() * sizeof(char) + sizeof(char));
    }
    memcpy(this->data, value.c_str(), value.length() * sizeof(char));
    this->data[value.length()] = '\0';
    len = value.length() * sizeof(char);
    this->type = TSTRING;
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

std::string Register::toString() {
    switch (this->type) {
        case TINT:
            return std::to_string(*(reinterpret_cast<int*>(this->data)));
            break;
        case TANY:
        case TSTRING:
        default:
            return std::string(this->data);
            break;
    }
}

Register::~Register(){
    free(this->data);
}
