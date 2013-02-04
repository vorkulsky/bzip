#pragma once
#ifndef decode
#define decode

#include "operation.h"

class Decode : public Operation
{
private:

public:
	Decode(const char* inFileName, const char* outFileName, int threadsQuantity):
		Operation(inFileName, outFileName, threadsQuantity) {};
	virtual ~Decode();
private:
	virtual void afterOpeningFiles();
	virtual void threadRun();
	virtual void readData();
	virtual void writeData();
};

#endif