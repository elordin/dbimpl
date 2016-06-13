#pragma once

#include <cstdint>
#include <string>
#include <string.h>

enum RegisterType {
	TINT,
	TSTRING,
	TANY
};


class Register {
 private:
 	size_t len;
	char* data;
	std::hash<std::string> hashfun;
	RegisterType type;
 public:

    Register();

	// retrieve an integer
	long getInteger();

	// store an integer
	void setInteger(long value);

	// retrieve a string
	std::string getString();

	// store a string
	void setString(const std::string& value);


	// Hash-operator for an integer
	uint64_t hash();

	bool operator==(Register &that);

	bool operator==(Register *that);

	std::string toString();

    ~Register();
};
