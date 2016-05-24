#include <cassert>

#include "BTree.hpp"


Key* BTree::nthKeyOfNode(char* node, unsigned n) {
    assert(n < fanOut);
    return reinterpret_cast<Key*>(node + sizeof(char*) + (sizeof(char*) + sizeof(Value)) * n);
}

char* BTree::nthPointerOfNode(char* node, unsigned n) {
    assert(n <= fanOut);
    return reinterpret_cast<Value*>(node + (sizeof(char*) + sizeof(Value)) * n);
}

char* BTree::findChildNode(char* node, Key key) {

}

Value* BTree::findEntry(char* node, Key key) {

}

bool BTree::isLeaf(char* node) {

}

bool BTree::contains(char* node, Key key) {

}

BTree::BTree() {

}

bool BTree::insert(Key key, Value value) {

}

bool BTree::erase(Key key) {

}

Value BTree::lookup(Key key) {
    std::function<Value(char*)> recLookup = [key, &recLookup](char* node) -> Value {
        if (BTree::isLeaf(node) && BTree::contains(node, key)) {
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

