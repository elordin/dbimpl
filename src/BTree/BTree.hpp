#pragma once

#include <cassert>
#include <cerrno>
#include <iostream>
#include <string>
#include <typeinfo>

#include "../BufferManager/BufferManager.hpp"
#include "../SlottedPages/TID.hpp"

#define FANOUT 4

template<class Key, class Value>
struct InnerNode {
    bool isLeaf;
    unsigned lsn;
    uint64_t upper; //page-id of right-most child
    unsigned num_keys;
    Key keys[FANOUT - 1];
    uint64_t children[FANOUT];

	InnerNode() : num_keys(0), isLeaf(false) {}
    uint64_t getChildPageId(Key key);
    bool contains(Key key);
};

template<class Key, class Value>
struct LeafNode {
    bool isLeaf;
    unsigned lsn;
    uint64_t next;
    unsigned num_keys;
    Key keys[FANOUT - 1];
    Value values[FANOUT - 1];

	LeafNode() : num_keys(0), isLeaf(true) {}
    unsigned indexOfKey(Key key);
    bool contains(Key key);
};

template<
    class Key,
    class Value,
    class Comparator
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
	unsigned size = 1;

 public:
    BTree<key_type, value_type, Comparator>(BufferManager* bm) : bm(bm) {
    };

	unsigned getSize() { return size; }

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
        Comparator c = Comparator();
		return c(key, key2);
	}

	void copyMemory(LeafNode<key_type, value_type>* leaf, unsigned destinationDiff, unsigned sourceDiff, unsigned index){
		memcpy(leaf->keys + destinationDiff * sizeof(key_type),
            leaf->keys + sourceDiff * sizeof(key_type),
        	(FANOUT - index) * sizeof(key_type)
		);
		memcpy(leaf->values + destinationDiff * sizeof(value_type),
        	leaf->values + sourceDiff * sizeof(value_type),
        	(FANOUT - index) * sizeof(value_type)
		);
	}

	void copyMemory(InnerNode<key_type, value_type>* node, unsigned destinationDiff, unsigned sourceDiff, unsigned index){
		memcpy(node->keys + destinationDiff * sizeof(key_type),
            node->keys + sourceDiff * sizeof(key_type),
        	(FANOUT - index) * sizeof(key_type)
		);
		memcpy(node->children + destinationDiff * sizeof(uint64_t),
        	node->children + sourceDiff * sizeof(uint64_t),
       		(FANOUT - index) * sizeof(uint64_t)
		);
	}

	void allocateMemory(LeafNode<key_type, value_type>* leaf, unsigned destinationDiff, unsigned index){
		memcpy(leaf->keys + destinationDiff * sizeof(key_type),
        	malloc(sizeof(key_type)),
            (FANOUT - index) * sizeof(key_type)
		);
		memcpy(leaf->values + destinationDiff * sizeof(value_type),
        	malloc(sizeof(key_type)),
       		(FANOUT - index) * sizeof(value_type)
		);
	}

	void allocateMemory(InnerNode<key_type, value_type>* node, unsigned destinationDiff, unsigned index){
		memcpy(node->keys + destinationDiff * sizeof(key_type),
        	malloc(sizeof(key_type)),
            (FANOUT - index) * sizeof(key_type)
		);
		memcpy(node->children + destinationDiff * sizeof(uint64_t),
        	malloc(sizeof(uint64_t)),
            (FANOUT - index) * sizeof(uint64_t)
		);
	}

    /**
     *  Inserts the given value with associated key into the tree.
     *  Throws an exception if the value was already in the tree.
     */
    bool insert(key_type key, value_type value) {
        // lookup leaf in which key should be inserted
		uint64_t pageId = this->rootPageId;
		uint64_t parentPageIds[this->size - 2];
		unsigned counter = 0;
        LeafNode<key_type, value_type>* leaf;
		InnerNode<key_type, value_type>* parents[this->size -2];
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
				parents[counter] = inner;
				parentPageIds[counter] = pageId;
				counter++;
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
					this->copyMemory(leaf, i+1, i, i);
              	}
                leaf->num_keys++;
                leaf->keys[place]= key;
                leaf->values[place]= value;
				this->allocateMemory(leaf, place+1, place);
				bm->unfixPage(*frame, true);
                return true;
          	} else {
            	// split leaf into two
                LeafNode<key_type, value_type>* new_leaf = new LeafNode<key_type, value_type>();
                new_leaf->num_keys = leaf->num_keys - (FANOUT/2);
                for(unsigned j=0; j < new_leaf->num_keys; ++j) {
                	new_leaf->keys[j] = leaf->keys[(FANOUT/2)+j];
                    new_leaf->values[j] = leaf->values[(FANOUT/2)+j];
					this->copyMemory(new_leaf, j+1, (FANOUT/2)+j+1, j);
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
						this->copyMemory(leaf, i+1, i, i);
                    }
                    leaf->num_keys++;
                    leaf->keys[place]= key;
                    leaf->values[place]= value;
					this->allocateMemory(leaf, place+1, place);
                } else {
                    place = place - (FANOUT / 2);
                    for(unsigned i=new_leaf->num_keys; i > place; --i) {
                    	new_leaf->keys[i]= new_leaf->keys[i-1];
                    	new_leaf->values[i]= new_leaf->values[i-1];
						this->copyMemory(new_leaf, i+1, i, i);
                    }
                    new_leaf->num_keys++;
                    new_leaf->keys[place]= key;
                    new_leaf->values[place]= value;
					this->allocateMemory(new_leaf, place+1, place);
                }
				// insert maximum of left page as separator into parent, if this is not the root
				key_type separator = leaf->keys[leaf->num_keys-1];
				if(!(this->rootPageId == frame->getPageNo())){
					this->insertSeparator(parents, parentPageIds, counter, separator, frame->getPageNo());

				} else {
                // create a new root if needed
					uint64_t new_pageId = bm->addPage();
					BufferFrame* new_frame = &bm->fixPage(new_pageId, true);
					InnerNode<key_type, value_type>* new_root = new InnerNode<key_type, value_type>();
					this->rootPageId = new_pageId;
					this->size++;
					new_root->keys[0] = separator;
					new_root->children[0] = frame->getPageNo();
					new_root->num_keys++;
					bm->unfixPage(*new_frame, true);
				}
				bm->unfixPage(*frame, true);
				return true;
        	}
    	}
    }

	bool insertSeparator(InnerNode<key_type, value_type>* parents[], uint64_t parentPageIds[], unsigned currentParent, key_type separator, uint64_t pageId){
		BufferFrame* frame = &bm->fixPage(parentPageIds[currentParent], true);
		InnerNode<key_type, value_type>* node = parents[currentParent];
		unsigned place = 0;
        while((place < node->num_keys) && (isSmaller(node->keys[place], separator))) {
     		++place;
       	}
        for(unsigned i=node->num_keys; i > place; --i) {
       		node->keys[i]= node->keys[i-1];
            node->children[i]= node->children[i-1];
			this->copyMemory(node, i+1, i, i);
     	}
        node->num_keys++;
        node->keys[place]= separator;
        node->children[place]= pageId;
		this->allocateMemory(node, place+1, place);
		// if parent overflows, split it
		if(node->num_keys > FANOUT - 1){
			// split parent
			InnerNode<key_type, value_type>* new_node = new InnerNode<key_type, value_type>();
            new_node->num_keys = node->num_keys - (FANOUT/2);
            for(unsigned j=0; j < new_node->num_keys; ++j) {
            	new_node->keys[j] = node->keys[(FANOUT/2)+j];
                new_node->children[j] = node->children[(FANOUT/2)+j];
				this->copyMemory(new_node, j+1, (FANOUT/2)+j+1, j);
            }
            node->num_keys = (FANOUT / 2);
            // insert separator into proper side
            unsigned place = 0;
            while((place < node->num_keys) && (isSmaller(node->keys[place], separator))) {
            	++place;
            }
            if(place < (FANOUT / 2)){
            	for(unsigned i=node->num_keys; i > place; --i) {
                	node->keys[i]= node->keys[i-1];
                    node->children[i]= node->children[i-1];
					this->copyMemory(node, i+1, i, i);
               	}
                node->num_keys++;
                node->keys[place]= separator;
                node->children[place]= pageId;
				this->allocateMemory(node, place+1, place);
            } else {
                place = place - (FANOUT / 2);
                for(unsigned i=new_node->num_keys; i > place; --i) {
                	new_node->keys[i]= new_node->keys[i-1];
                    new_node->children[i]= new_node->children[i-1];
					this->copyMemory(new_node, i+1, i, i);
               	}
                new_node->num_keys++;
                new_node->keys[place]= separator;
                new_node->children[place]= pageId;
				allocateMemory(new_node, place+1, place);
            }
			key_type new_separator = node->keys[node->num_keys-1];
			if(!(this->rootPageId == frame->getPageNo())){
				this->insertSeparator(parents, parentPageIds, currentParent-1, new_separator, frame->getPageNo());
			} else {
			// create a new root if needed
				uint64_t new_pageId = bm->addPage();
				BufferFrame* new_frame = &bm->fixPage(new_pageId, true);
				InnerNode<key_type, value_type>* new_root = new InnerNode<key_type, value_type>();
				this->rootPageId = new_pageId;
				this->size++;
				new_root->keys[0] = new_separator;
				new_root->children[0] = frame->getPageNo();
				new_root->num_keys++;
				bm->unfixPage(*new_frame, true);
			}
		}
		bm->unfixPage(*frame, true);
        return true;
	}

    bool isLeaf(void* data) {
        return *reinterpret_cast<bool*>(data);
    }

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

	//Lookup adjusted to test
	bool lookup(key_type key, TID tid){
		Value* result = this->lookup(key);
		uint64_t result_tid = reinterpret_cast<uint64_t>(result);
		if(tid.getTID() == result_tid){
			return true;
		} else {
			return false;
		}
	}

    /**
     *  Returns the value associated with key from the tree.
     *  Throws an exception if the value cannot be found.
     */
	bool lookup(key_type key, value_type &tid){
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
            tid = node->values[node->indexOfKey(key)];
            return true;
        } else {
            return false;
        }

    }

};
