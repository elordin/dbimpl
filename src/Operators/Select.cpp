#include <cstdint>
#include <string>
#include <string.h>

#include "Select.hpp"

using namespace std;

Select::Select(Operator* input, Register* attribute, string value) {
	this->input = input;
	this->attribute = attribute;
	this->value = value;
}

void Select::open() {
	this->input->open();
}

bool Select::next(){
	while(input->next()){
		//TODO: get value of the attribute
		string s = attribute->getString();
		if(s == value){
			return true;
		}
	}
	return false;
}

vector<Register*> Select::getOutput(){
	return this->input->getOutput();
}

void Select::close(){
	this->input->close();

}

Select::~Select(){

}
