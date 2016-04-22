#include "BufferManager.hpp"
#include "BufferFrame.hpp"
#include "HashTable.hpp"


using namespace std;

BufferManager::BufferManager(uint pageCount)
  : table(new HashTable()),
    framesInMemory(pageCount) {
}

BufferFrame& BufferManager::fixPage(uint64_t pageId, bool exclusive) {
    if (this->table->contains(pageId)) {  // If page is in memory
        if (exclusive) {
            // If no one else has locks
                // Allow
            // If others have locks
                // Resolve
        } else {
            // If only other non-exclusive locks
                // Allow
            // If exclusively locked
                // Resolve
        }
    } else {  // If page is not in memory
        // Load page into memory
        // If loading into memory fails 'cause no free space is available and no page can be evicted
            // Fail by throwning an exception
    }
    // Set locks
}

void BufferManager::unfixPage(BufferFrame& frame, bool isDirty) {
    // Release one lock.
            // Can only be a single write or multiple read locks. Write is removed, read is decreased.
    // If no one uses the page anymore
        if (isDirty) {
            // Write changes to disc
        }
        // Flag for eviction
}


BufferManager::~BufferManager() {
    // Delete all buffer frames
    // Delete hash table
}
