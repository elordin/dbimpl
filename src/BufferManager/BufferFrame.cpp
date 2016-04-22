#pragma once

#include "BufferFrame.hpp"

uint64_t BufferFrame::countPages = 0;

//TODO: control if the page number rises
BufferFrame::BufferFrame(void* dataForFrame){
	pageNo = ++countPages;
	data = dataForFrame;
}

void* BufferFrame::getData(){
	return data;
}

void writeBackChanges(){
	insertItem(this);
}

//TODO: Müssen wir da noch was anderes freigeben?
BufferFrame::~BufferFrame(){
	delete [] data;
}
