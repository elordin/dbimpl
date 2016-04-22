#include <iostream>
#include <unordered_map>
#include "BufferFrame.hpp"
#include "HashTable.hpp"

using namespace std;


HashTable::HashTable()
{
	unordered_map<uint64_t,BufferFrame*> ht;
	hashtable = ht;
	//TODO: not static	
	length = 128;
}

int HashTable::hash(uint64_t key)
{
	return key % length;	
}

void HashTable::insertItem(BufferFrame* newItem)
{
    	int i = hash(newItem -> getPageNo());
    	hashtable.emplace(i, newItem);
	//TODO: error message: cout << "Item could not be inserted!" << endl;
}

void HashTable::removeItem(uint64_t key)
{
    	int i = hash(key);
    	hashtable.erase(i);
	//TODO: error message: cout << "Item could not be removed!" << endl;
}

BufferFrame* HashTable::getItemByKey(uint64_t key)
{
    	int i = hash(key);
    	return hashtable[i];
}

HashTable::~HashTable()
{
    	//TODO: Destructor
}


