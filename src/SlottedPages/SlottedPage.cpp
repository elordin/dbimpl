#include "SlottedPage.hpp"

#define SLOT_SIZE 2

using namespace std;

SlottedPage::SlottedPage(unsigned pageSize)
  : pageSize(pageSize){
	header = 0;
    //TODO: firstSlot;
    firstEmptySlot = firstSlot;
    //TODO: slotEnd;
    end = pageSize;
    freeSpace = end;
}

uint64_t SlottedPage::insert(const Record& r){
	Slot* slot = firstEmptySlot;
	//TODO: pointer to data	
	firstEmptySlot >> SLOT_SIZE;
	freeSpace << r.getLen();
	//TODO: return offset
}

void SlottedPage::remove(uint64_t slot){
	
}
		
uint64_t SlottedPage::recompress(){

}

Slot* SlottedPage::getSlot(uint64_t slotNum){

}

Slot* SlottedPage::getFreeSlot(){
	return firstEmptySlot;
}

Record* SlottedPage::getRecordPtr(uint64_t slotNum){

}

unsigned SlottedPage::getFreeSpaceOnPage(){
	return freeSpace-slotEnd;
}
	
SlottedPage::~SlottedPage(){

}
