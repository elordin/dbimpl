#pragma once

#include<HashTable.hpp>

HashTable::HashTable(int tableLength)
{
    	if (tableLength <= 0) 
		tableLength = 128;
    	array = new LinkedList[tableLength];
    	length = tableLength;
}

int HashTable::hash(uint64_t key)
{
	return key % length;	
}

void HashTable::insertItem(BufferFrame* newItem)
{
    	int i = hash(newItem -> key);
    	if(!array[i].insertItem(newItem)){
		cout << "Item could not be inserted!" << endl;
	}
}

void HashTable::removeItem(uint64_t key)
{
    	int i = hash(key);
    	if(!array[i].removeItem(key)){
		cout << "Item could not be removed!" << endl;
	}
}

BufferFrame* HashTable::getItemByKey(uint64_t key)
{
    	int i = hash(key);
    	return array[i].getItem(key);
}

HashTable::~HashTable()
{
    	delete [] array;
}
