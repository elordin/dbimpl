#include <mutex>
#include <iostream>
#include <unordered_map>
#include "BufferFrame.hpp"
#include "HashTable.hpp"

using namespace std;


HashTable::HashTable() {}


void HashTable::lockBucket(uint64_t key) {
    // TODO
}


void HashTable::unlockBucket(uint64_t key) {
    // TODO
}


void HashTable::lockTable() {
    this->global_mutex.lock();
}


void HashTable::unlockTable() {
    this->global_mutex.unlock();
}


bool HashTable::contains(uint64_t key) {
    this->lockTable();
    bool contains = this->hashtable.count(key) == 1;
    this->unlockTable();
    return contains;
}


BufferFrame& HashTable::get(uint64_t key) {
    return *this->hashtable.at(key);
}


void HashTable::insert(uint64_t key, BufferFrame* value) {
    this->lockBucket(key);
    hashtable.insert({{key, value}});
    this->unlockBucket(key);
}


void HashTable::remove(uint64_t key) {
    this->lockBucket(key);
    hashtable.erase(key);
    this->unlockBucket(key);
}


uint HashTable::size() {
    this->lockTable();
    uint size = this->hashtable.size(); // Count entries
    this->unlockTable();
    return size;
}


HashTable::~HashTable() {
    // TODO Delete all frames.
}


