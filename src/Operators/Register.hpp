#pragma once

#include <cstdint>
#include <string>
#include <string.h>

class Register {
 private:
	
 public:
    
    Register();

	// retrieve an integer
	uint64_t getInteger();

	// store an integer
	void setInteger(uint64_t value);
		
	// retrieve a string
	std::string getString();
	
	// store a string
	void setString(const std::string& value);

	//TODO: operator ==, <

	// Hash-operator for an integer
	uint64_t hash(uint64_t value);

	// Hash-operator for a string
	uint64_t hash(std::string& value);
    
    ~Register();
};
