#include <cerrno>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "TID.hpp"

using namespace std;

TID::TID(uint64_t pageID, uint64_t slotID)
  : pageID(pageID),
    slotID(slotID){

}

uint64_t TID::getPage(){
	return this->pageID;
}

uint64_t TID::getSlot(){
	return this->slotID;
}

TID::~TID(){

}
