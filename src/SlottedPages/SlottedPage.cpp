#include "SlottedPage.hpp"

using namespace std;

SlottedPage::SlottedPage(){
 
}

uint64_t SlottedPage::insert(const Record& r){

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
