#include <cassert>
#include <cerrno>
#include <iostream>
#include <functional>

#include "BTree.hpp"

using namespace std;

Key* BTree::nthKeyOfNode(char* node, unsigned n) {
    assert(n < fanOut);
    return reinterpret_cast<Key*>(node + sizeof(char*) + (sizeof(char*) + sizeof(Value)) * n);
}

char* BTree::nthPointerOfNode(char* node, unsigned n) {
    assert(n <= fanOut);
    return reinterpret_cast<Value*>(node + (sizeof(char*) + sizeof(Value)) * n);
}

char* BTree::findChildNode(char* node, key_type key) {

}

value_type* BTree::getEntry(char* node, key_type key) {

}

bool BTree::isLeaf(char* node) {

}

bool BTree::contains(char* node, key_type key) {
    if(!isLeaf(node)){
        perror("Node is not a leaf node.");
        throw "Node is not a leaf node.";
    } else {
        LeafNode<Key, Value, fanOut>* leaf = reinterpret_cast<LeafNode<Key, Value, fanOut>*>(node);
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

BTree::BTree() {

}

bool BTree::insert(key_type key, value_type value) {
    // lookup leaf in which key should be inserted
    function<Value(char*)> recFindNode = [key, value, &recFindNode](char* node) -> Value {
        if(this->isLeaf(node)){
            // if key is already in the tree, throw error
            if(contains(node, key)){
                perror("Key already in tree.");
                throw "Key already in tree.";
            } else {
                LeafNode<Key, Value, fanOut>* leaf = reinterpret_cast<LeafNode<Key, Value, fanOut>*>(node);
                // if there is enough space in the leaf (fanOut-2 at most)
                if(leaf->num_keys <= fanOut-2){
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
                    LeafNode<Key, Value, fanOut>* new_leaf = new LeafNode<Key, Value, fanOut>();
                    new_leaf->num_keys = leaf->num_keys - (fanOut/2);
                    for(unsigned j=0; j < new_leaf->num_keys; ++j) {
                        new_leaf->keys[j] = leaf->keys[(fanOut/2)+j];
                        new_leaf->values[j] = leaf->values[(fanOut/2)+j];
                    }
                    leaf->num_keys = (fanOut/2);
                    // insert entry into proper side
                    unsigned place = 0;
                    while((place < leaf->num_keys) && (leaf->keys[place]<key)) {
                        ++place;
                    }
                    if(place < (fanOut/2)){
                        for(unsigned i=leaf->num_keys; i > place; --i) {
                            leaf->keys[i]= leaf->keys[i-1];
                            leaf->values[i]= leaf->values[i-1];
                        }
                        leaf->num_keys++;
                        leaf->keys[place]= key;
                        leaf->values[place]= value;
                    } else {
                        place = place - (fanOut/2);
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

bool BTree::erase(key_type key) {

}

value_type* BTree::lookup(key_type key) {
    function<value_type*(char*)> recLookup = [key, &recLookup](char* node) -> value_type* {
        if (this->isLeaf(node) && this->contains(node, key)) {
            // Key found
            return BTree::getEntry(node, key);
        } else if (BTree::isLeaf(node)) {
            // Key is not in the tree
            throw "Element not found.";
        } else {
            // Need to go deeper
            return recLookup(BTree::findChildNode(node, key));
        }
    };
    return recLookup(this->root);
}

