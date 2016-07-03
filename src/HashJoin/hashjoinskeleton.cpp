#include <iostream>
#include <cstdlib>
#include <atomic>
#include <mutex>
#include <tbb/tbb.h>
#include <unordered_map>

using namespace tbb;
using namespace std;

inline uint64_t hashKey(uint64_t k) {
   // MurmurHash64A
   const uint64_t m = 0xc6a4a7935bd1e995;
   const int r = 47;
   uint64_t h = 0x8445d61a4e774912 ^ (8*m);
   k *= m;
   k ^= k >> r;
   k *= m;
   h ^= k;
   h *= m;
   h ^= h >> r;
   h *= m;
   h ^= h >> r;
   return h|(1ull<<63);
}

class ChainingLockingHT {
    // Chained tuple entry
    public: struct Entry {
        uint64_t key;
        uint64_t value;
        Entry* next;
    };

    struct Chain {
        std::mutex lock;
        Entry* head;
        uint64_t size;
        Chain() : head(nullptr), size(0) {}
    };
    private:
        unordered_map<uint64_t, Chain*> table;
    public:

    // Constructor
    ChainingLockingHT(uint64_t size) {
        this->table = unordered_map<uint64_t, Chain*>();
    }

    // Destructor
    ~ChainingLockingHT() {
    }

    // Returns the number of hits
    inline uint64_t lookup(uint64_t key) {
        return this->table[hashKey(key)]->size;
    }

    inline void insert(Entry* entry) {
        uint64_t key = hashKey(entry->key);
		Chain* chain;
		if(this->table.find(key) == this->table.end()){
			entry->next = nullptr;
			chain = new Chain();
			chain->head = entry;
			chain->size++;
			this->table.emplace(key, chain);
		} else {
			Chain* chain = this->table[key];
			std::lock_guard<std::mutex> lock(chain->lock);

        	Entry* curr = chain->head;
        	while (curr->next != nullptr) {
            	curr = curr->next;
        	}
       		curr->next = entry;
        	chain->size++;
		}
    }
};

class ChainingHT {

   // Chained tuple entry
   public: struct Entry {
      uint64_t key;
      uint64_t value;
      Entry* next;
   };

	private:
        unordered_map<uint64_t, ChainingHT::Entry*> hashTable;

	public:
		std::atomic<Entry*> nextOfCurrentEntry;

   // Constructor
   ChainingHT(uint64_t size){
		this->hashTable = unordered_map<uint64_t, ChainingHT::Entry*>(size);
   }

   // Destructor
   ~ChainingHT() {
   }

   // Returns the number of hits
   inline uint64_t lookup(uint64_t key) {
		uint64_t hashedKey = hashKey(key);
		if(this->hashTable.count(hashedKey) < 1){
			return 0;
		} else {
			Entry* e = hashTable.at(hashedKey);
			unsigned counter = 1;
			while(e->next != nullptr){
				counter++;
				e = e->next;
			}
			return counter;
		}
   }

   inline void insert(Entry* entry) {
      uint64_t hashValue = hashKey(entry->key);
		// if hashValue indicates an empty bucket, just insert
		if(this->hashTable.find(hashValue) == this->hashTable.end()){
			entry->next = nullptr;
			hashTable.emplace(hashValue, entry);
		} else {
			entry->next = nullptr;
			Entry* currentEntry = hashTable.at(hashValue);
			while(currentEntry->next != nullptr){
				currentEntry = currentEntry->next;
			}
			Entry* tmp = 0;
			nextOfCurrentEntry.store(currentEntry->next);
			while(!nextOfCurrentEntry.compare_exchange_weak(tmp, entry, memory_order_release, memory_order_relaxed));
			currentEntry->next = nextOfCurrentEntry.load();
			hashTable.emplace(hashValue, entry);
		}
   }
};

class LinearProbingHT {
   public:
   // Entry
   struct Entry {
      uint64_t key;
      uint64_t value;
      std::atomic<bool> marker;
      Entry(uint64_t key, uint64_t value) : key(key), value(value) {
        marker.store(false);
      }
   };

   // Constructor
   LinearProbingHT(uint64_t size) {
   }

   // Destructor
   ~LinearProbingHT() {
   }

   // Returns the number of hits
   inline uint64_t lookup(uint64_t key) {
        uint64_t hkey = hashKey(key);
        uint64_t h = hkey;
        uint64_t c = 1;
        Entry* e = this->table[h];

        while (e != nullptr) {
            e = this->table[++h];
            c++;
        }

        // while (e != nullptr && e->marker.load() && e->key == hkey) {
            // e = this->table[++h];
            // if (e->key == hkey) c++;
        // }
        return c;
   }

   inline void insert(uint64_t key, Entry* value) {
        uint64_t hkey = hashKey(key);
        Entry* e = this->table[hkey];
        if (e == nullptr) {
            value->marker.store(false);
            this->table.emplace(hkey, value);
        } else {
            uint64_t h = hkey;
            while (e->key != hkey && e->marker.load()) {
                e = this->table[++h];
                if (e == nullptr) {
                    std::cout << h << std::endl;
                    this->table.emplace(h, value);
                    break;
                }
            }
        }
   }

    private:
        unordered_map<uint64_t, Entry*> table;
};

int main(int argc,char** argv) {
   uint64_t sizeR = atoi(argv[1]);
   uint64_t sizeS = atoi(argv[2]);
   unsigned threadCount = atoi(argv[3]);

   task_scheduler_init init(threadCount);

   // Init build-side relation R with random data
   uint64_t* R=static_cast<uint64_t*>(malloc(sizeR*sizeof(uint64_t)));
   parallel_for(blocked_range<size_t>(0, sizeR), [&](const blocked_range<size_t>& range) {
         unsigned int seed=range.begin();
         for (size_t i=range.begin(); i!=range.end(); ++i)
            R[i]=rand_r(&seed)%sizeR;
      });

   // Init probe-side relation S with random data
   uint64_t* S=static_cast<uint64_t*>(malloc(sizeS*sizeof(uint64_t)));
   parallel_for(blocked_range<size_t>(0, sizeS), [&](const blocked_range<size_t>& range) {
         unsigned int seed=range.begin();
         for (size_t i=range.begin(); i!=range.end(); ++i)
            S[i]=rand_r(&seed)%sizeR;
      });

   // STL
   {
      // Build hash table (single threaded)
      tick_count buildTS=tick_count::now();
      unordered_multimap<uint64_t,uint64_t> ht(sizeR);
      for (uint64_t i=0; i<sizeR; i++)
         ht.emplace(R[i],0);
      tick_count probeTS=tick_count::now();
      cout << "STL      	    build:" << (sizeR/1e6)/(probeTS-buildTS).seconds() << "MT/s ";

      // Probe hash table and count number of hits
      std::atomic<uint64_t> hitCounter;
      hitCounter=0;
      parallel_for(blocked_range<size_t>(0, sizeS), [&](const blocked_range<size_t>& range) {
            uint64_t localHitCounter=0;
            for (size_t i=range.begin(); i!=range.end(); ++i) {
               auto range=ht.equal_range(S[i]);
               for (unordered_multimap<uint64_t,uint64_t>::iterator it=range.first; it!=range.second; ++it)
                  localHitCounter++;
            }
            hitCounter+=localHitCounter;
         });
      tick_count stopTS=tick_count::now();
      cout << "probe: " << (sizeS/1e6)/(stopTS-probeTS).seconds() << "MT/s "
           << "total: " << ((sizeR+sizeS)/1e6)/(stopTS-buildTS).seconds() << "MT/s "
           << "count: " << hitCounter << endl;
   }

   // Test of ChainingLockingHT
   {
	  // Build hash table
      tick_count buildTS=tick_count::now();
      ChainingLockingHT* cl(new ChainingLockingHT(sizeR));
	  parallel_for(blocked_range<size_t>(0, sizeR), [&](const blocked_range<size_t>& range) {
			for (size_t i=range.begin(); i!=range.end(); ++i) {
                ChainingLockingHT::Entry* e(new ChainingLockingHT::Entry());
		 		e->key = R[i];
		 		e->value = 0;	//see your STL Test
        		cl->insert(e);
            }
	  	 });
	  tick_count probeTS=tick_count::now();
      cout << "ChainingLockingHT   build:" << (sizeR/1e6)/(probeTS-buildTS).seconds() << "MT/s ";

	  // Probe hash table and count number of hits
      std::atomic<uint64_t> hitCounter;
      hitCounter=0;
      parallel_for(blocked_range<size_t>(0, sizeS), [&](const blocked_range<size_t>& range) {
            uint64_t localHitCounter=0;
			for (size_t i=range.begin(); i!=range.end(); ++i) {
  	           localHitCounter += cl->lookup(S[i]);
            }
            hitCounter+=localHitCounter;
         });
      tick_count stopTS=tick_count::now();
      cout << "probe: " << (sizeS/1e6)/(stopTS-probeTS).seconds() << "MT/s "
           << "total: " << ((sizeR+sizeS)/1e6)/(stopTS-buildTS).seconds() << "MT/s "
           << "count: " << hitCounter << endl;
	}

   // Test of ChainingHT
   {
	  // Build hash table
      tick_count buildTS=tick_count::now();
      ChainingHT* c(new ChainingHT(sizeR));
	  parallel_for(blocked_range<size_t>(0, sizeR), [&](const blocked_range<size_t>& range) {
			for (size_t i=range.begin(); i!=range.end(); ++i) {
                ChainingHT::Entry* e(new ChainingHT::Entry());
		 		e->key = R[i];
		 		e->value = 0;	//see your STL Test
         		c->insert(e);
            }
	  	 });
	  tick_count probeTS=tick_count::now();
      cout << "ChainingHT          build:" << (sizeR/1e6)/(probeTS-buildTS).seconds() << "MT/s ";

	  // Probe hash table and count number of hits
      std::atomic<uint64_t> hitCounter;
      hitCounter=0;
      parallel_for(blocked_range<size_t>(0, sizeS), [&](const blocked_range<size_t>& range) {
            uint64_t localHitCounter=0;
			for (size_t i=range.begin(); i!=range.end(); ++i) {
               localHitCounter += c->lookup(S[i]);
            }
            hitCounter+=localHitCounter;
         });
      tick_count stopTS=tick_count::now();
      cout << "probe: " << (sizeS/1e6)/(stopTS-probeTS).seconds() << "MT/s "
           << "total: " << ((sizeR+sizeS)/1e6)/(stopTS-buildTS).seconds() << "MT/s "
           << "count: " << hitCounter << endl;
	}

   // Test of LinearProbing
   {
	  // Build hash table
      tick_count buildTS=tick_count::now();
      LinearProbingHT* lp(new LinearProbingHT(sizeR));
	  parallel_for(blocked_range<size_t>(0, sizeR), [&](const blocked_range<size_t>& range) {
			for (size_t i=range.begin(); i!=range.end(); ++i) {
         		lp->insert(R[i], new LinearProbingHT::Entry(R[i], 0));
            }
	  	 });
	  tick_count probeTS=tick_count::now();
      cout << "LinearProbingHT     build:" << (sizeR/1e6)/(probeTS-buildTS).seconds() << "MT/s ";

	  // Probe hash table and count number of hits
      std::atomic<uint64_t> hitCounter;
      hitCounter=0;
      parallel_for(blocked_range<size_t>(0, sizeS), [&](const blocked_range<size_t>& range) {
            uint64_t localHitCounter=0;
			for (size_t i=range.begin(); i!=range.end(); ++i) {
               localHitCounter += lp->lookup(S[i]);
            }
            hitCounter+=localHitCounter;
         });
      tick_count stopTS=tick_count::now();
      cout << "probe: " << (sizeS/1e6)/(stopTS-probeTS).seconds() << "MT/s "
           << "total: " << ((sizeR+sizeS)/1e6)/(stopTS-buildTS).seconds() << "MT/s "
           << "count: " << hitCounter << endl;
	}

   return 0;
}
