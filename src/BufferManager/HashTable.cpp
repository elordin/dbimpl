#include <iostream>
#include <unordered_map>
#include "BufferFrame.hpp"
#include "HashTable.hpp"

using namespace std;


HashTable::HashTable() {}


void lockBucket(uint64_t key) {
    // TODO
}


void unlockBucket(uint64_t key) {
    // TODO
}


void lockTable() {
    this->global_mutex.lock();
}


void unlockTable() {
    this->global_mutex.unlock();
}


bool contains(uint64_t key) {
    this->lockTable();
    bool contains = this->table.count(key) == 1;
    this->unlockTable();
    return contains;
}


BufferFrame& HashTable::get(uint64_t key) {
    return hashtable.at(key);
}


void HashTable::insert(uint64_t key, BufferFrame value) {
    this->lockBucket(key);
    hashtable.emplace(key, value);
    this->unlockBucket(key);
}


void HashTable::remove(uint64_t key) {
    this->lockBucket(key);
    hashtable.erase(key);
    this->unlockBucket(key);
}


uint size() {
    this->lockTable();
    uint size = this->table.size(); // Count entries
    this->unlockTable();
    return size;
}


HashTable::~HashTable() {
    // TODO Delete all frames.
}


