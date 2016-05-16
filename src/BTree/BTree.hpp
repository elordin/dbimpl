#pragma once

#include "../SlottedPage/TID.hpp"

template<
    class Key,
    class KeyEqual = std::equal_to<Key>
>
class BTree {
 private:
 public:
    BTree();

    bool insert(Key key, TID tid);

    bool erase(Key key);

    TID lookup(Key key);

}
