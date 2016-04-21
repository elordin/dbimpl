#pragma once

#include<BufferManager.hpp>

BufferManager::BufferManager(uint pageCount){
	//TODO: static value?	
	table = new HashTable(128);
	framesInMemory = pageCount;
}

BufferFrame& BufferManager::fixPage(uint64_t pageId, bool exclusive){

}

void BufferManager::unfixPage(BufferFrame& frame, bool isDirty){

}

BufferManager::~BufferManager(){
	delete [] table;
}
