#include "BTree.hpp"


char* BTree::findChildNode(char* node, Key key) {

}

Value BTree::getEntry(char* node, Key key) {

}

bool BTree::isLeaf(char* node) {

}

bool BTree::contains(char* node, Key key) {

}

BTree::BTree(unsigned fanOut) {

}

bool BTree::insert(Key key, Value value) {

}

bool BTree::erase(Key key) {

}

Value BTree::lookup(Key key) {
    std::function<Value(char*)> recLookup = [key, &recLookup](char* node) -> Value {
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

