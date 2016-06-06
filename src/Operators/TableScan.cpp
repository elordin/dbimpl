#include <cstdint>
#include <vector>

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
		// load tuple t from in at position tid
		currentTuple = input.lookup(tid).getData();
		tid++;
		return true;
	}
}

vector<Register*> TableScan::getOutput(){
	// read tuple from in at position tid
	next();
	Register r;
	r.setString(currentTuple);
	//TODO: get more than 1 attribute
	vector<Register*> vector = {&r};
	return vector;
}

void TableScan::close(){

}
    
TableScan::~TableScan(){

}
