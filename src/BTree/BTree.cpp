#include <cerrno>
#include <iostream>
#include <functional>
#include "BTree.hpp"

using namespace std;

template<class Key, class Value, unsigned fanOut>
char* BTree<Key, Value, fanOut>::findChildNode(char* node, Key key) {

}

template<class Key, class Value, unsigned fanOut>
Value BTree<Key, Value, fanOut>::getEntry(char* node, Key key) {

}

template<class Key, class Value, unsigned fanOut>
bool BTree<Key, Value, fanOut>::isLeaf(char* node) {

}

template<class Key, class Value, unsigned fanOut>
bool BTree<Key, Value, fanOut>::contains(char* node, Key key) {
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

template<class Key, class Value, unsigned fanOut>
BTree<Key, Value, fanOut>::BTree() {

}

template<class Key, class Value, unsigned fanOut>
bool BTree<Key, Value, fanOut>::insert(Key key, Value value) {
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
					// insert entry into proper side
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

template<class Key, class Value, unsigned fanOut>
bool BTree<Key, Value, fanOut>::erase(Key key) {

}

template<class Key, class Value, unsigned fanOut>
Value BTree<Key, Value, fanOut>::lookup(Key key) {
    function<Value(char*)> recLookup = [key, &recLookup](char* node) -> Value {
        if (this->isLeaf(node) && this->contains(node, key)) {
            // Key found
            return this->getEntry(node, key);
        } else if (this->isLeaf(node)) {
            // Key is not in the tree
            throw "Element not found.";
        } else {
            // Need to go deeper
            return recLookup(this->findChildNode(node, key));
        }
    };
    return recLookup(this->root);
}

