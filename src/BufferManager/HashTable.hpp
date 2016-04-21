#pragma once

#include <cstdint>
#include<Linked_List.h>

class HashTable {
	
private:
	LinkedList* array;
	int length;
	int hash(uint64_t key);

public:
    
	//Constructor	
	//TODO: length not static
	HashTable(int tableLength = 128);
    
    	void insertItem(Item* newItem);
    
    	bool removeItem(uint64_t key);
    
    	Item* getItemByKey(uint64_t key);
    
    	// Destructor
    	~HashTable();
};

