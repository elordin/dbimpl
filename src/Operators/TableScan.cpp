#include <cstdint>
#include <vector>

#include "TableScan.hpp"
#include "../SlottedPages/Record.hpp"

using namespace std;

TableScan::TableScan(SPSegment* relation)
	: input(relation){

}

void TableScan::open() {
	tid = 0;
	limit = this->input->getLastTID().tid;
}

bool TableScan::next() {
	if (tid >= limit) return false;

	Record r = input->inPlaceLookup(TID(tid));
	if (r.getLen() < 1) return this->next();

	/*
	// TODO Solve the recursion as a loop
	Record r = input->inPlaceLookup(TID(tid++));
	while (r.getLen() < 1) {
		r = input->inPlaceLookup(TID(tid++));
	}
	*/

	currentTuple = r.getData();
	return true;
}

vector<Register*> TableScan::getOutput() {
	return this->input->toRegisterVector(this->currentTuple);
}

void TableScan::close() {}

TableScan::~TableScan() {}
