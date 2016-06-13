#include <cstdint>

#include "HashJoin.hpp"

using namespace std;

HashJoin::HashJoin(Operator* leftOp, Operator* rightOp, unsigned regIdLeft, unsigned regIdRight)
: leftOp(leftOp),
  rightOp(rightOp),
  regIdLeft(regIdLeft),
  regIdRight(regIdRight) {
	
}

void HashJoin::open(){
	this->leftOp->open();
	this->rightOp->open();
	// build hashtable for the left side
	while(leftOp->next()){
		std::vector<Register*> tuple = leftOp->getOutput();
        Register* attributeValue = tuple[this->regIdLeft];
		uint64_t hashValue = attributeValue->hash();
        // place tuple in hash table by hashValue
		pair<uint64_t, vector<Register*>> pair (hashValue, tuple);
		this->hashTable.insert(pair);
	}
}

bool HashJoin::next(){

	// probe right side
	while(rightOp->next()){
		std::vector<Register*> tuple = rightOp->getOutput();
        Register* attributeValue = tuple[this->regIdRight];
		uint64_t hashValue = attributeValue->hash();
		// if hashValue indicates a nonempty bucket of the hash table		
		if(!(this->hashTable.find(hashValue) == this->hashTable.end())){
			// if hashValue matches any s in the bucket
			vector<Register*> matchTuple = this->hashTable.at(hashValue);
			Register* match = matchTuple[this->regIdLeft];
			if(match == attributeValue){
				// store r and s in result
				vector<Register*>::iterator it;
				it = result.begin();	
				for(unsigned i=0; i<tuple.size(); i++){			
					it = result.insert(it, tuple[i]);
				}
				for(unsigned j=0; j<matchTuple.size(); j++){	
					it = result.insert(it, matchTuple[j]);
				}
				return true;
			}
		}
	}

	return false;
}

vector<Register*> HashJoin::getOutput(){
	return this->result;
}

void HashJoin::close(){
	this->leftOp->close();
	this->rightOp->close();
}
    
HashJoin::~HashJoin(){

}
