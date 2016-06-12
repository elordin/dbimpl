#include <cstdint>
#include <vector>

#include "TableScan.hpp"

using namespace std;

TableScan::TableScan(SPSegment* relation)
	: input(relation){

}

void TableScan::open() {
	tid = 0;
	// limit = TODO;
}

bool TableScan::next() {
	if (tid < limit) {
		currentTuple = input->lookup(tid++).getData();
		return true;
	} else {
		return false;
	}
}

vector<Register*> TableScan::getOutput() {
	return this->input->toRegisterVector(this->currentTuple);
}

void TableScan::close() {}

TableScan::~TableScan() {}
