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

#define FANOUT 2;

template<
    class Key,
    class Value,
	unsigned fanOut
    //, class KeyEqual= std::equal_to<Key>
>
class BTree {

    typedef Key     key_type;
    typedef Value   value_type;

    struct Node {
        Node* children[FANOUT];
        Key keys[FANOUT - 1];
    }

 private:
    char* root;

    /**
     *  Returns the nth element in a node.
     */
    static Key* nthKeyOfNode(char* node, unsigned n);

    /**
     *  Returns nth pointer within a node, pointing to nth child.
     */
    static char* nthPointerOfNode(char* node, unsigned n);

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
    static bool contains(char* node, key_type key);

 public:
    BTree();

    /**
     *  Inserts the given value with associated key into the tree.
     *  Throws an exception if the value was already in the tree.
     */
    bool insert(key_type key, value_type value);

    /**
     *  Removes the entry with value key from the tree.
     */
    bool erase(key_type key);

    /**
     *  Returns the value associated with key from the tree.
     *  Throws an exception if the value cannot be found.
     */
    Value* lookup(key_type key);

};
