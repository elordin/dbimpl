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

    void lockBucket(uint64_t pageId);
    void unlockBucket(uint64_t pageId);

    void lockTable();
    void unlockTable();

    /**
     *  Returns whether a given key is in the hash table
     */
    bool contains(uint64_t); // TODO Thread safety

    BufferFrame& get(uint64_t); // TODO Thread safety

    void insert(uint64_t, const BufferFrame&);
    void remove(uint64_t);

    uint size(); // TODO Thread safety

    int hash(uint64_t key);

    void insertItem(BufferFrame* newItem);

    void removeItem(uint64_t key);

    BufferFrame* getItemByKey(uint64_t key);

    // Destructor
    ~HashTable();
};

