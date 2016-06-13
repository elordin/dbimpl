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
	if (tid < limit) {

		// TODO Skip empty records
		// TODO Skip indirected records

		currentTuple = input->lookup(TID(tid)).getData();
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
