#pragma once
#ifndef encode
#define encode

#include "operation.h"

class Encode : public Operation
{
private:
    int _blockSize;
public:
    Encode(const char* inFileName, const char* outFileName, int threadsQuantity, int blockSize):
        Operation(inFileName, outFileName, threadsQuantity), _blockSize(blockSize) {};
    virtual ~Encode();
private:
    virtual void afterOpeningFiles();
    virtual void threadRun();
    virtual void readData();
    virtual void writeData();
};

#endif
