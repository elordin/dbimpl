#pragma once

#include <cassert>
#include <cerrno>
#include <iostream>
#include <functional>

#define FANOUT 2

template<class Key, class Value>
struct InnerNode {
	InnerNode() : num_keys(0) {}
	unsigned lsn;
	uint64_t upper; //page-id of right-most child
	unsigned num_keys;
	Key keys[FANOUT - 1];
	//char* children[fanOut];
};

template<class Key, class Value>
struct LeafNode {
	LeafNode() : num_keys(0) {}
	unsigned lsn;
	uint64_t next;
	unsigned num_keys;
	Key keys[FANOUT - 1];
	Value values[FANOUT - 1];
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
    char* root;

    /**
     *  Returns the nth element in a node.
     */
    static key_type* nthKeyOfNode(char* node, unsigned n) {
        return reinterpret_cast<key_type*>(node + sizeof(char*) + (sizeof(char*) + sizeof(Value)) * n);
    }

    /**
     *  Returns nth pointer within a node, pointing to nth child.
     */
    static char* nthPointerOfNode(char* node, unsigned n) {
        return reinterpret_cast<value_type*>(node + (sizeof(char*) + sizeof(value_type)) * n);
    }

    /**
     *  Returns child node that would presumably contain key.
     *  Throws an exception if node is a leaf node.
     */
    static char* findChildNode(char* node, key_type key);

    /**
     *  Returns the entry with given key from given node.
     *  Throws an exception if node is not a leaf node.
     */
    static value_type* findEntry(char* node, key_type key);

    /**
     *  Returns whether a given node is a leaf node or not.
     */
    static bool isLeaf(char* node);

    /**
     *  Returns whether a given (leaf) node contains entry with given key.
     *  Throws an exception if node is not a leaf node.
     */
    static bool contains(char* node, key_type key) {
        if(!isLeaf(node)){
            perror("Node is not a leaf node.");
            throw "Node is not a leaf node.";
        } else {
            LeafNode<key_type, value_type>* leaf = reinterpret_cast<LeafNode<Key, Value>*>(node);
            unsigned place = 0;
            while((place < leaf->num_keys-1) && (leaf->keys[place]<key)) {
                ++place;
            }
            if(leaf->keys[place+1] == key){
                return true;
            } else {
                return false;
            }
        }
    }

 public:
    BTree();

    /**
     *  Inserts the given value with associated key into the tree.
     *  Throws an exception if the value was already in the tree.
     */
    bool insert(key_type key, value_type value) {
        // lookup leaf in which key should be inserted
        std::function<Value(char*)> recFindNode = [this, key, value, &recFindNode](char* node) -> Value {
            if(this->isLeaf(node)){
                // if key is already in the tree, throw error
                if(contains(node, key)){
                    perror("Key already in tree.");
                    throw "Key already in tree.";
                } else {
                    LeafNode<key_type, value_type>* leaf = 
                        reinterpret_cast<LeafNode<key_type, value_type>*>(node);
                    // if there is enough space in the leaf (fanOut-2 at most)
                    if(leaf->num_keys <= FANOUT - 2){
                        // insert key and value in the appropriate place
                        unsigned place = 0;
                        while((place < leaf->num_keys) && (leaf->keys[place]<key)) {
                            ++place;
                        }
                        for(unsigned i=leaf->num_keys; i > place; --i) {
                            leaf->keys[i]= leaf->keys[i-1];
                            leaf->values[i]= leaf->values[i-1];
                        }
                        leaf->num_keys++;
                        leaf->keys[place]= key;
                        leaf->values[place]= value;
                        return true;
                    } else {
                        // split leaf into two
                        LeafNode<key_type, value_type>* new_leaf = 
                            new LeafNode<key_type, value_type>();
                        new_leaf->num_keys = leaf->num_keys - (FANOUT/2);
                        for(unsigned j=0; j < new_leaf->num_keys; ++j) {
                            new_leaf->keys[j] = leaf->keys[(FANOUT/2)+j];
                            new_leaf->values[j] = leaf->values[(FANOUT/2)+j];
                        }
                        leaf->num_keys = (FANOUT / 2);
                        // insert entry into proper side
                        unsigned place = 0;
                        while((place < leaf->num_keys) && (leaf->keys[place]<key)) {
                            ++place;
                        }
                        if(place < (FANOUT / 2)){
                            for(unsigned i=leaf->num_keys; i > place; --i) {
                                leaf->keys[i]= leaf->keys[i-1];
                                leaf->values[i]= leaf->values[i-1];
                            }
                            leaf->num_keys++;
                            leaf->keys[place]= key;
                            leaf->values[place]= value;
                        } else {
                            place = place - (FANOUT / 2);
                            for(unsigned i=new_leaf->num_keys; i > place; --i) {
                                new_leaf->keys[i]= new_leaf->keys[i-1];
                                new_leaf->values[i]= new_leaf->values[i-1];
                            }
                            new_leaf->num_keys++;
                            new_leaf->keys[place]= key;
                            new_leaf->values[place]= value;
                        }
                        // recSeparator: insert maximum of left page as separator into parent
                        // if parent overflows
                            // split parent
                            // recSeparator()
                        // create a new root if needed
                    }
                }
            } else {
                return recFindNode(this->findChildNode(node, key));
            }
        };
        return recFindNode(this->root);
    }

    /**
     *  Removes the entry with value key from the tree.
     */
    bool erase(key_type key) {
        // Find node

        // Check for underflow

        // If no underflow, delete

    }

    /**
     *  Returns the page from the tree presumably containing the key.
     */
    char* pageLookup(key_type key) {
        std::function<LeafNode<key_type, value_type>*(char*)> recLookup = 
            [this, key, &recLookup](char* node) -> LeafNode<key_type, value_type>* {
            if (this->isLeaf(node)) {
                return node;
            } else {
                return recLookup(findChildNode(node, key));
            }
        };
        return recLookup(this->root);
    }

    /**
     *  Returns the value associated with key from the tree.
     *  Throws an exception if the value cannot be found.
     */
    Value* lookup(key_type key) {
        LeafNode<key_type, value_type>* node = this->pageLookup(key);
        if (this->contains(node, key)) {
            return this->getEntry(node, key);
        } else {
            throw "Element not found.";        
        }
    }

};
