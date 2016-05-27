#pragma once

#include <cassert>
#include <cerrno>
#include <iostream>
#include <string>
#include <typeinfo>

#include "../BufferManager/BufferManager.hpp"
#include "../SlottedPages/TID.hpp"

#define FANOUT 2

template<class Key, class Value>
struct InnerNode {
	InnerNode() : num_keys(0) {}
	unsigned lsn;
	uint64_t upper; //page-id of right-most child
	unsigned num_keys;
	Key keys[FANOUT - 1];
	uint64_t children[FANOUT];

    uint64_t getChildPageId(Key key);
    bool contains(Key key);
};

template<class Key, class Value>
struct LeafNode {
	LeafNode() : num_keys(0) {}
	unsigned lsn;
	uint64_t next;
	unsigned num_keys;
	Key keys[FANOUT - 1];
	Value values[FANOUT - 1];

    unsigned indexOfKey(Key key);
    bool contains(Key key);
};

template<
    class Key,
    class Value
    //, class KeyEqual= std::equal_to<Key>
>
class BTree {

    typedef Key     key_type;
    typedef Value   value_type;

    struct Node {
        Node* children[FANOUT];
        key_type keys[FANOUT - 1];
    };

 private:
    uint64_t rootPageId;
    BufferManager* bm;

 public:
    BTree<key_type, value_type>(BufferManager* bm) : bm(bm) {
    };

	//TODO
	unsigned size(){}

	//TODO
	unsigned findEntryByIndex(LeafNode<key_type, value_type>* node, key_type key){}

	//TODO
	bool isSmaller(key_type key, key_type key2){
		/*if(typeid(key).name() =='m') {
			return key < key2;
		} else if(typeid(key).name() == 'c'){
			return memcmp(key.data, key2.data, 20) < 0;
		} else if(typeid(key).name() == "St4pairIiiE"){
			if (key.first < key2.first){
         		return true;
      		} else {
         		return (key.first == key2.first) && (key.second < key2.second);
			}
		}*/
		return false;
	}

    /**
     *  Inserts the given value with associated key into the tree.
     *  Throws an exception if the value was already in the tree.
     */
    bool insert(key_type key, value_type value) {
        // lookup leaf in which key should be inserted
		uint64_t pageId = this->rootPageId;
        LeafNode<key_type, value_type>* leaf;
        BufferFrame* frame = (BufferFrame*) malloc(sizeof(BufferFrame));

        bool searching = true;
        while (searching) {
            frame = &(bm->fixPage(pageId, false));
            auto data = frame->getData();
            if (isLeaf(data)) {
                leaf = reinterpret_cast<LeafNode<key_type, value_type>*>(data);
				searching = false;
            } else {
                InnerNode<key_type, value_type>* inner = reinterpret_cast<InnerNode<key_type, value_type>*>(data);
                pageId = inner->getChildPageId(key);
                bm->unfixPage(*frame, false);
            }
        }

        // if key is already in the tree, throw error
        if(leaf->contains(key)){
        	perror("Key already in tree.");
            throw "Key already in tree.";
        } else {
			bm->unfixPage(*frame, false);
            frame = &bm->fixPage(pageId, true);

            // if there is enough space in the leaf (fanOut-2 at most)
            if(leaf->num_keys <= FANOUT - 2){
            	// insert key and value in the appropriate place
                unsigned place = 0;
                while((place < leaf->num_keys) && (isSmaller(leaf->keys[place], key))) {
                	++place;
                }
                for(unsigned i=leaf->num_keys; i > place; --i) {
                    leaf->keys[i]= leaf->keys[i-1];
                	leaf->values[i]= leaf->values[i-1];
					memcpy(leaf->keys + (i + 1) * sizeof(key_type),
                    	leaf->keys + (i) * sizeof(key_type),
                    	(FANOUT - i) * sizeof(key_type)
					);
					memcpy(leaf->values + (i + 1) * sizeof(value_type),
                    	leaf->values + (i) * sizeof(value_type),
                    	(FANOUT - i) * sizeof(value_type)
					);
              	}
                leaf->num_keys++;
                leaf->keys[place]= key;
                leaf->values[place]= value;
				memcpy(leaf->keys + (place + 1) * sizeof(key_type),
                	malloc(sizeof(key_type)),
                    (FANOUT - place) * sizeof(key_type)
				);
				memcpy(leaf->values + (place + 1) * sizeof(value_type),
                    malloc(sizeof(key_type)),
                    (FANOUT - place) * sizeof(value_type)
				);
				bm->unfixPage(*frame, true);
                return true;
          	} else {
            	// split leaf into two
                LeafNode<key_type, value_type>* new_leaf = new LeafNode<key_type, value_type>();
                new_leaf->num_keys = leaf->num_keys - (FANOUT/2);
                for(unsigned j=0; j < new_leaf->num_keys; ++j) {
                	new_leaf->keys[j] = leaf->keys[(FANOUT/2)+j];
                    new_leaf->values[j] = leaf->values[(FANOUT/2)+j];
					memcpy(new_leaf->keys + (j + 1) * sizeof(key_type),
                    	new_leaf->keys + ((FANOUT/2) + j + 1) * sizeof(key_type),
                    	(FANOUT - j) * sizeof(key_type)
					);
					memcpy(new_leaf->values + (j + 1) * sizeof(value_type),
                    	new_leaf->values + ((FANOUT/2) + j + 1) * sizeof(value_type),
                    	(FANOUT - j) * sizeof(value_type)
					);
                }
                leaf->num_keys = (FANOUT / 2);
                // insert entry into proper side
                unsigned place = 0;
                while((place < leaf->num_keys) && (isSmaller(leaf->keys[place], key))) {
                	++place;
                }
                if(place < (FANOUT / 2)){
                	for(unsigned i=leaf->num_keys; i > place; --i) {
                    	leaf->keys[i]= leaf->keys[i-1];
                    	leaf->values[i]= leaf->values[i-1];
						memcpy(leaf->keys + (i + 1) * sizeof(key_type),
		                	leaf->keys + (i) * sizeof(key_type),
		                	(FANOUT - i) * sizeof(key_type)
						);
						memcpy(leaf->values + (i + 1) * sizeof(value_type),
		                	leaf->values + (i) * sizeof(value_type),
		                	(FANOUT - i) * sizeof(value_type)
						);
                    }
                    leaf->num_keys++;
                    leaf->keys[place]= key;
                    leaf->values[place]= value;
					memcpy(leaf->keys + (place + 1) * sizeof(key_type),
                		malloc(sizeof(key_type)),
                    	(FANOUT - place) * sizeof(key_type)
					);
					memcpy(leaf->values + (place + 1) * sizeof(value_type),
                    	malloc(sizeof(key_type)),
                    	(FANOUT - place) * sizeof(value_type)
					);
                } else {
                    place = place - (FANOUT / 2);
                    for(unsigned i=new_leaf->num_keys; i > place; --i) {
                    	new_leaf->keys[i]= new_leaf->keys[i-1];
                    	new_leaf->values[i]= new_leaf->values[i-1];
						memcpy(new_leaf->keys + (i + 1) * sizeof(key_type),
		                	new_leaf->keys + (i) * sizeof(key_type),
		                	(FANOUT - i) * sizeof(key_type)
						);
						memcpy(new_leaf->values + (i + 1) * sizeof(value_type),
		                	new_leaf->values + (i) * sizeof(value_type),
		                	(FANOUT - i) * sizeof(value_type)
						);
                    }
                    new_leaf->num_keys++;
                    new_leaf->keys[place]= key;
                    new_leaf->values[place]= value;
					memcpy(new_leaf->keys + (place + 1) * sizeof(key_type),
                		malloc(sizeof(key_type)),
                    	(FANOUT - place) * sizeof(key_type)
					);
					memcpy(new_leaf->values + (place + 1) * sizeof(value_type),
                    	malloc(sizeof(key_type)),
                    	(FANOUT - place) * sizeof(value_type)
					);
                }
                // recSeparator: insert maximum of left page as separator into parent
                	// if parent overflows
                    // split parent
                    // recSeparator()
                // create a new root if needed
				bm->unfixPage(*frame, true);
				return true;
        	}
    	}
    }

    // TODO
    bool isLeaf(void* data) { return false; }

    /**
     *  Removes the entry with value key from the tree.
     */
    bool erase(key_type key) {
        // Find node

        uint64_t pageId = this->rootPageId;
        LeafNode<key_type, value_type>* node;

        BufferFrame* frame = (BufferFrame*) malloc(sizeof(BufferFrame));

        bool notFound = true;
        while (notFound) {
            frame = &(bm->fixPage(pageId, false));
            auto data = frame->getData();
            if ((notFound = isLeaf(data))) {
                node = reinterpret_cast<LeafNode<key_type, value_type>*>(data);
            } else {
                InnerNode<key_type, value_type>* inner = reinterpret_cast<InnerNode<key_type, value_type>*>(data);
                pageId = inner->getChildPageId(key);
                bm->unfixPage(*frame, false);
            }
        }

        if (node->num_keys > 0 && node->contains(key)) {
            bm->unfixPage(*frame, false);
            frame = &bm->fixPage(pageId, true);
            // Check for underflow
            // if (node->num_keys == FANOUT / 2) {
                // Underflow
            // } else {
                // TODO Currently ignoring underflow
                unsigned index = this->findEntryByIndex(node, key);
                node->num_keys--;
                memcpy(node->values + index * sizeof(value_type),
                    node->values + (index + 1) * sizeof(value_type),
                    (FANOUT - 1 - index) * sizeof(value_type)
                );
                memcpy(node->keys + index * sizeof(key_type),
                    node->keys + (index + 1) * sizeof(key_type),
                    (FANOUT - 1 - index) * sizeof(value_type)
                );
                bm->unfixPage(*frame, true);
                return true;
            // }
        } else {
            bm->unfixPage(*frame, false);
            return false;
        }
    }

	//TODO: Adjust to test, use tid
	Value* lookup(key_type key, TID tid){}

    /**
     *  Returns the value associated with key from the tree.
     *  Throws an exception if the value cannot be found.
     */
    Value* lookup(key_type key) {
        // Find node
        uint64_t pageId = this->rootPageId;
        LeafNode<key_type, value_type>* node;
        BufferFrame* frame = (BufferFrame*) malloc(sizeof(BufferFrame));
        bool notFound = true;

        while (notFound) {
            frame = &(bm->fixPage(pageId, false));
            auto data = frame->getData();
            if ((notFound = isLeaf(data))) {
                node = reinterpret_cast<LeafNode<key_type, value_type>*>(data);
            } else {
                InnerNode<key_type, value_type>* inner = reinterpret_cast<InnerNode<key_type, value_type>*>(data);
                pageId = inner->getChildPageId(key);
                bm->unfixPage(*frame, false);
            }
        }
        if (node->num_keys > 0 && node->contains(key)) {
            Value* ret = (Value*) malloc(sizeof(Value));
            *ret = node->values[node->indexOfKey(key)];
            return ret;
        } else {
            throw "Not found.";
        }

    }

};
