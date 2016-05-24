#include <cassert>
#include <cerrno>
#include <iostream>
#include <functional>

#include "BTree.hpp"

using namespace std;

void dummy() {
    BTree<int, int>* tree = new BTree<int, int>();
}