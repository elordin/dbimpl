#pragma once

#include <mutex>
#include <cstdint>
#include <unordered_map>

#include "BufferFrame.hpp"


class HashTable {
 private:
    std::unordered_map<uint64_t, BufferFrame*> hashtable;

    std::mutex global_mutex;

 public:
    // Constructor
    HashTable();

    void lockBucket(uint64_t key);

    void unlockBucket(uint64_t key);

    void lockTable();

    void unlockTable();

    /**
     *  Returns whether a given key is in the hash table
     */
    bool contains(uint64_t); // TODO Thread safety

    BufferFrame& get(uint64_t); // TODO Thread safety

    void insert(uint64_t, BufferFrame*);

    void remove(uint64_t);

    uint size(); // TODO Thread safety

    ~HashTable();
};

