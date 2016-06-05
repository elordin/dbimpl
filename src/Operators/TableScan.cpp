#include <cstdint>

#include "TableScan.hpp"

using namespace std;

TableScan::TableScan(SPSegment relation)
	: input(relation){

}

void TableScan::open(SPSegment relation){
	input = relation;
	tid = 0;
	limit = input.getPageSize();
}

bool TableScan::next(){
	if(tid >= limit-1){
		return false;
	} else {
		tid++;
		// load tuple t from in at position tid
		input.lookup(tid);
		return true;
	}
}

vector<Register*> TableScan::getOutput(){
	// read tuple from in at position tid
	// something like: input.lookup(tid).getData();
}

void TableScan::close(){

}
    
TableScan::~TableScan(){

}
