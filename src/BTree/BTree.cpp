#include <cassert>
#include <cerrno>
#include <iostream>
#include <functional>

#include "BTree.hpp"

using namespace std;

struct MyCustomUIntCmp {
   bool operator()(int a, int b) const {
      return a<b;
   }
};

void dummy() {
    BTree<int, int, MyCustomUIntCmp>* tree = new BTree<int, int, MyCustomUIntCmp>(new BufferManager(100));
}