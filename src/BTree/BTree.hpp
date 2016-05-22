#pragma once

#include "../SlottedPages/TID.hpp"

template<class Key, class Value, unsigned fanOut>
struct InnerNode {
	InnerNode() : num_keys(0) {}
	unsigned lsn;
	uint64_t upper; //page-id of right-most child
	unsigned num_keys;
	Key keys[fanOut-1];
	//char* children[fanOut];
};

template<class Key, class Value, unsigned fanOut>
struct LeafNode {
	LeafNode() : num_keys(0) {}
	unsigned lsn;
	uint64_t next;
	unsigned num_keys;
	Key keys[fanOut-1];
	Value values[fanOut-1];
};

template<
    class Key,
    class Value,
	unsigned fanOut
    //, class KeyEqual= std::equal_to<Key>
>

class BTree {
 private:
    char* root;

    /**
     *  Returns child node that would presumably contain key.
     *  Throws an exception if node is a leaf node.
     */
    char* findChildNode(char* node, Key key);

    /**
     *  Returns the entry with given key from given node.
     *  Throws an exception if node is not a leaf node.
     */
    Value getEntry(char* node, Key key);

    /**
     *  Returns whether a given node is a leaf node or not.
     */
    bool isLeaf(char* node);

    /**
     *  Returns whether a given (leaf) node contains entry with given key.
     *  Throws an exception if node is not a leaf node.
     */
    bool contains(char* node, Key key);
 public:
	BTree();

    /**
     *  Inserts the given value with associated key into the tree.
     *  Throws an exception if the value was already in the tree.
     */
    bool insert(Key key, Value value);

    /**
     *  Removes the entry with value key from the tree.
     */
    bool erase(Key key);

    /**
     *  Returns the value associated with key from the tree.
     *  Throws an exception if the value cannot be found.
     */
    Value lookup(Key key);

};
