
#include "BufferManager.hpp"
#include "BufferFrame.hpp"
#include "HashTable.hpp"


using namespace std;

BufferManager::BufferManager(uint pageCount){
	//TODO: static value?	
	HashTable *table = new HashTable();
	uint framesInMemory = pageCount;
}

BufferFrame& BufferManager::fixPage(uint64_t pageId, bool exclusive){
	//TODO: The method can fail (by throwing an exception) if no free frame is available and no used frame can be freed	
	if(exclusive){
		// return getItemByKey(pageId);
	} else {
		// return getItemByKey(pageId);
	}
}

void BufferManager::unfixPage(BufferFrame& frame, bool isDirty){
	if(isDirty){
		// removeItem(frame.getPageNo());
		// frame.writeBackChanges();
	}
}

//TODO: destructor
BufferManager::~BufferManager(){
	
}