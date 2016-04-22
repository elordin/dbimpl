#pragma once

#include "BufferManager.hpp"

BufferManager::BufferManager(uint pageCount){
	//TODO: static value?	
	table = new HashTable(128);
	framesInMemory = pageCount;
}

BufferFrame& BufferManager::fixPage(uint64_t pageId, bool exclusive){
	//TODO: The method can fail (by throwing an exception) if no free frame is available and no used frame can be freed	
	if(exclusive){
		BufferFrame* bf = getItemByKey(pageId);
		bf.getData();
	} else {
		BufferFrame* bf = getItemByKey(pageId);
		bf.getData();
	}
}

void BufferManager::unfixPage(BufferFrame& frame, bool isDirty){
	if(isDirty){
		remove(frame.getPageNo());
		frame.writeBackChanges();
	}
}

BufferManager::~BufferManager(){
	delete [] table;
}
