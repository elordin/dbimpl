#include "TID.hpp"

using namespace std;


TID::TID(uint64_t tid) : tid(tid) {}

uint64_t TID::getPage() {
    return (this->tid >> 8) & 0x00FFFFFFFFFFFFFFL;
}

uint64_t TID::getSlot() {
    return this->tid & 0x00000000000000FFL;
}

TID::~TID(){

}
