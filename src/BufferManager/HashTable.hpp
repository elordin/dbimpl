#pragma once

#include <cstdint>
#include <unordered_map>

#include "BufferFrame.hpp"


class HashTable {
 private:
    std::unordered_map<uint64_t, BufferFrame*> hashtable;
    uint64_t length;

 public:
    // Constructor
    HashTable();

    /**
     *  Returns whether a given key is in the hash table
     */
    bool contains(uint64_t);

    BufferFrame* get(uint64_t);

    BufferFrame* operator[] (uint64_t);
    void operator+ (BufferFrame);

    int hash(uint64_t key);

    void insertItem(BufferFrame* newItem);

    void removeItem(uint64_t key);

    BufferFrame* getItemByKey(uint64_t key);

    // Destructor
    ~HashTable();
};

