#include <cstdint>

#include "TableScan.hpp"

using namespace std;

TableScan::TableScan(SPSegment relation)
	: relation(relation){

}

void TableScan::open(){

}

bool TableScan::next(){

}

vector<Register*> TableScan::getOutput(){

}

void TableScan::close(){

}
    
TableScan::~TableScan(){

}
